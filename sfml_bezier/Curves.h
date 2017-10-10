#pragma once
#include <vector>

float max(const float& a, const float& b) {
	if (a > b) return a;
	return b;
}

float min(const float& a, const float& b) {
	if (a < b) return a;
	return b;
}

sf::Vector2f lerp(float t, const sf::Vector2f a, const sf::Vector2f b) {
	return a + t * (b - a);
}

void lerp(float t, sf::Vector2f& res, const sf::Vector2f& a, const sf::Vector2f& b) {
	res =  a + t * (b - a);
}

bool AABB(const sf::Vector2f& p, const sf::Vector2f& tl, const sf::Vector2f& br) {
	return p.x > tl.x && p.x < br.x && p.y > tl.y && p.y < br.y;
}

bool MouseInPoint(const sf::RectangleShape& point, const sf::Vector2i& mpos, const int& pointsize) {
	sf::Vector2f tl(point.getPosition().x - pointsize / 2, point.getPosition().y - pointsize / 2);
	sf::Vector2f br(point.getPosition().x + pointsize / 2, point.getPosition().y + pointsize / 2);
	return AABB(static_cast<sf::Vector2f>(mpos), tl, br);
}

float length(const sf::Vector2f& p0, const sf::Vector2f& p1) {
	return abs(sqrtf(((p1.x - p0.x)*(p1.x - p0.x))
		+ ((p1.y - p0.y)*(p1.y - p0.y))));
}

void normalize(sf::Vector2f& source)
{
	float length = sqrt((source.x * source.x) + (source.y * source.y));
	if (length != 0)
		source = sf::Vector2f(source.x / length, source.y / length);
}

class Line {
public:
	sf::Vector2f p0, p1;
	sf::Vertex* line;
	Line() {
		p0.x = 0;
		p0.y = 0;
		p1.x = 0;
		p1.y = 0;
	}
	Line(float x0, float y0, float x1, float y1) {
		p0.x = x0;
		p0.y = y0;
		p1.x = x1;
		p1.y = y1;
		line = new sf::Vertex[2]{
			sf::Vertex(p0), sf::Vertex(p1)
		};
	}
	void Update(float x0, float y0, float x1, float y1) {
		p0.x = x0;
		p0.y = y0;
		p1.x = x1;
		p1.y = y1;
		line[0] = p0;
		line[1] = p1;
	}
};

class DiscreteCurve {
protected:
	std::vector<sf::RectangleShape*> points;
	std::vector<Line*> lines;
	int _npoints, nlines, pointsize;
	void MovePoint(sf::RectangleShape* currentPoint, int currentPointID, const sf::Vector2f& pos) {
		currentPoint->setPosition(pos);
		if (currentPointID == 0) {
			lines[0]->Update(currentPoint->getPosition().x, currentPoint->getPosition().y,
				lines[0]->p1.x, lines[0]->p1.y);
		}
		else if (currentPointID == _npoints - 1) {
			lines[currentPointID - 1]->Update(lines[currentPointID - 1]->p0.x, lines[currentPointID - 1]->p0.y,
				currentPoint->getPosition().x, currentPoint->getPosition().y);
		}
		else {
			lines[currentPointID - 1]->Update(lines[currentPointID - 1]->p0.x, lines[currentPointID - 1]->p0.y,
				currentPoint->getPosition().x, currentPoint->getPosition().y);
			lines[currentPointID]->Update(currentPoint->getPosition().x, currentPoint->getPosition().y,
				lines[currentPointID]->p1.x, lines[currentPointID]->p1.y);
		}
	}
public:
	DiscreteCurve(int npoints, int pointsize, sf::Vector2f p0, sf::Vector2f p1) 
		: _npoints(npoints), nlines(npoints - 1){
		this->pointsize = pointsize;
		for (int i = 0; i < npoints; i++) {
			sf::RectangleShape* p = new sf::RectangleShape(sf::Vector2f(pointsize, pointsize));
			p->setFillColor(sf::Color::Red);
			p->setOrigin(pointsize / 2, pointsize / 2);
			p->setPosition(lerp((float)i / (npoints - 1), p0, p1));
			points.push_back(p);
		}
		for (int i = 0; i < nlines; i++) {
			Line* l = new Line(points[i]->getPosition().x, points[i]->getPosition().y,
				points[i + 1]->getPosition().x, points[i + 1]->getPosition().y);
			lines.push_back(l);
		}
	}
	int nPoints() { return _npoints; }
	int nLines() { return nlines; }
	bool drawPoints = true;
	virtual void draw(sf::RenderWindow& window) {
		if(pointsize > 0 && drawPoints)
			for (int i = 0; i < _npoints; i++) window.draw(*points[i]);
		for (int i = 0; i < nlines; i++) window.draw(lines[i]->line, 2, sf::Lines);
	}
};

class InteractiveDiscreteCurve : public DiscreteCurve{
private:
	sf::RectangleShape* currentPoint = nullptr;
	int currentPointID = 0;
public:
	InteractiveDiscreteCurve(int npoints, int pointsize, sf::Vector2f p0, sf::Vector2f p1)
	: DiscreteCurve(npoints, pointsize, p0, p1){ }
	virtual void update(bool lmbp, bool lmbr, const sf::Vector2i& mpos) {
		if (lmbr) currentPoint = nullptr;
		if (lmbp) {
			for (int i = 0; i < _npoints; i++) {
				if (MouseInPoint(*points[i], mpos, pointsize)) {
					currentPoint = points[i];
					currentPointID = i;
					break;
				}
			}
		}
		if (currentPoint != nullptr) MovePoint(currentPoint, currentPointID, static_cast<sf::Vector2f>(mpos));
	}
};

class Controls {
private:
	int size, nodesize, nlines;
	sf::RectangleShape* nodes;
	sf::RectangleShape* currentNode = nullptr;
	int currentNodeID = 0;
	Line* lines;
public:
	Controls(int size, int nodesize, sf::Vector2f* positions)
	: size(size), nodesize(nodesize){
		nodes = new sf::RectangleShape[size];
		for (int i = 0; i < size; i++) {
			nodes[i] = sf::RectangleShape(sf::Vector2f(nodesize, nodesize));
			nodes[i].setPosition(positions[i]);
			nodes[i].setOrigin(sf::Vector2f(nodesize/2, nodesize/2));
			nodes[i].setFillColor(sf::Color::Blue);
		}
		nlines = size - 1;
		lines = new Line[nlines];
		for (int i = 0; i < nlines; i++) {
			lines[i] = Line(positions[i].x, positions[i].y, positions[i + 1].x, positions[i + 1].y);
		}
	}
	void update(bool lmbp, bool lmbr, const sf::Vector2i& mpos) {
		if (lmbr) currentNode = nullptr;
		if (lmbp) {
			for (int i = 0; i < size; i++) {
				if (MouseInPoint(nodes[i], mpos, nodesize)) {
					currentNode = &nodes[i];
					currentNodeID = i;
					break;
				}
			}
		}
		if (currentNode != nullptr) {
			currentNode->setPosition(mpos.x, mpos.y);
			for (int i = 0; i < nlines; i++) {
				lines[i].Update(nodes[i].getPosition().x, nodes[i].getPosition().y,
					nodes[i + 1].getPosition().x, nodes[i + 1].getPosition().y);
			}
		}
	}
	bool drawLoop = true;
	void draw(sf::RenderWindow& window) {
		for (int i = 0; i < size; i++) window.draw(nodes[i]);
		if(drawLoop) for (int i = 0; i < nlines; i++) window.draw(lines[i].line, 2, sf::Lines);
	}
	sf::Vector2f getNode(int i) {
		return nodes[i].getPosition();
	}
};

class SnakeCurve : public DiscreteCurve {
private:
	int maxlen;
	float speed, maxspeed;
	Controls* controls;
public:
	SnakeCurve(int npoints, int pointsize, float speed, float maxspeed, sf::Vector2f p0, sf::Vector2f p1)
		: DiscreteCurve(npoints, pointsize, p0, p1){
		maxlen = length(p0, p1) / npoints;
		this->speed = speed;
		this->maxspeed = maxspeed;
		controls = new Controls(1, pointsize, new sf::Vector2f[1]{ p0 });
	}
	void update(bool lmbp, bool lmbr, const sf::Vector2i& mpos) {
		controls->update(lmbp, lmbr, mpos);
		MovePoint(points[0], 0, controls->getNode(0));
		for (int i = 1; i < _npoints; i++) {
			float len = length(points[i-1]->getPosition(), points[i]->getPosition());
			if (len > maxlen) {
				float dist = len - maxlen;
				sf::Vector2f p0 = points[i - 1]->getPosition();
				sf::Vector2f p1 = points[i]->getPosition();
				sf::Vector2f dir(p0.x - p1.x, p0.y - p1.y);
				normalize(dir);
				dir.x *= min(dist * speed, dist * speed * maxspeed);
				dir.y *= min(dist * speed, dist * speed * maxspeed);
				dir.x += p1.x;
				dir.y += p1.y;
				MovePoint(points[i], i, dir);
			}
		}
	}
	void draw(sf::RenderWindow& window) override {
		DiscreteCurve::draw(window);
		controls->draw(window);
	}
};

class Bezier : public DiscreteCurve {
private:
	Controls* controls;
	int degree;
	sf::Vector2f* bezierLerp(float t, sf::Vector2f* old, int oldsize) {
		sf::Vector2f* newvals = new sf::Vector2f[oldsize - 1];
		for (int i = 0; i < oldsize - 1; i++) {
			newvals[i] = lerp(t, old[i], old[i + 1]);
		}
		return newvals;
	}
	sf::Vector2f bezierPoint(float t) {
		sf::Vector2f* oldv, *newv;
		oldv = new sf::Vector2f[degree + 1];
		for (int i = 0; i < degree + 1; i++) oldv[i] = controls->getNode(i);
		for (int i = degree + 1; i > 1; i--) {
			newv = bezierLerp(t, oldv, i);
			delete oldv;
			oldv = newv;
		}
		sf::Vector2f res = oldv[0];
		delete oldv;
		return res;
	}
public:
	Bezier(int degree, int npoints, int pointsize, sf::Vector2f* vertices)
		: DiscreteCurve(npoints, pointsize, vertices[0], vertices[degree]) 
	, degree(degree) {
		controls = new Controls(degree + 1, pointsize, vertices);
		drawPoints = false;
	}
	Bezier(int degree, int npoints, int pointsize, sf::Vector2f p0, sf::Vector2f p1)
		: DiscreteCurve(npoints, pointsize, p0, p1)
		, degree(degree) {
		float len = length(p0, p1) / npoints;
		sf::Vector2f* vertices = new sf::Vector2f[degree + 1];
		for (int i = 0; i < degree + 1; i++) {
			vertices[i] = lerp(i * len / (len * (degree + 1)), p0, p1);
		}
		controls = new Controls(degree + 1, pointsize, vertices);
		drawPoints = false;
	}
	void DrawLoop(bool b) {
		controls->drawLoop = b;
	}
	void update(bool lmbp, bool lmbr, const sf::Vector2i& mpos) {
		controls->update(lmbp, lmbr, mpos);
		MovePoint(points[0], 0, controls->getNode(0));
		MovePoint(points[_npoints-1], _npoints-1, controls->getNode(degree));
		for (int i = 1; i < _npoints - 1; i++) {
			sf::Vector2f p = bezierPoint((float)i / _npoints);
			MovePoint(points[i], i, p);
		}
	}
	void draw(sf::RenderWindow& window) override {
		DiscreteCurve::draw(window);
		if (pointsize > 0)
			controls->draw(window);
	}
};