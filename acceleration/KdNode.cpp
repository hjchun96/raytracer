#include <acceleration/KdNode.h>
#include <acceleration/BoundingBox.h>
#include <vector>
#include <algorithm>
#include <scene/geometry/triangle.h>
#include <raytracer/intersection.h>

KDNode::KDNode() : left(nullptr), right(nullptr) {}

bool KDNode::intersect(const Ray &ray, Intersection *intersection) const {
	float t;
	if (!bbox.intersect(ray, t)) {
		return false;
	}

	if (left != nullptr && right != nullptr) {
		Intersection tmp;
		bool leftHit = left->intersect(ray, &tmp);
		bool rightHit = right->intersect(ray, &tmp);
        if ((leftHit || rightHit) &&
                ((intersection->t < 0 && tmp.t > 0) || tmp.t < intersection->t)) {
			intersection->isectPoint = tmp.isectPoint;
			intersection->normal = tmp.normal;
			intersection->objectHit = tmp.objectHit;
			intersection->t = tmp.t;
		}
		return leftHit || rightHit;
	} else {
		bool hit = false;
		Intersection curr, tmp;
		for (const auto &p : geometries) {
			if (p->intersect(ray, &tmp)) {
                if (tmp.t < curr.t && tmp.t > 0) {
					curr = tmp;
					hit = true;
				}
			}
		}
        if (hit && curr.t < intersection->t && curr.t > 0) {
			intersection->isectPoint = curr.isectPoint;
			intersection->normal = curr.normal;
			intersection->objectHit = curr.objectHit;
			intersection->t = curr.t;
		}
		return hit;
	}
}

KDNode *KDNode::build(std::vector<Geometry *> &geometries, int depth, int maxDepth) {
	KDNode *node = new KDNode();
	node->bbox = BoundingBox(geometries);

	if (geometries.size() < 5 || depth > maxDepth) {
		node->geometries = geometries;
		return node;
	}

	Axis longestAxis = node->bbox.getLongestAxis();
	std::vector<Geometry *> rightTriangles;
	std::vector<Geometry *> leftTriangles;
	std::vector<float> vals;
	for (const auto &p : geometries) {
		switch (longestAxis) {
		case Axis::X:
			vals.push_back(p->bbox.getMidpoint().x);
			break;
		case Axis::Y:
			vals.push_back(p->bbox.getMidpoint().y);
			break;
		case Axis::Z:
			vals.push_back(p->bbox.getMidpoint().z);
			break;
		}
	}
	std::nth_element(vals.begin(), vals.begin() + (vals.size() / 2), vals.end());
	float median = vals[vals.size() / 2];
	vals.clear();
	for (const auto &p : geometries) {
		switch (longestAxis) {
		case Axis::X:
			median < p->bbox.getMidpoint().x ? leftTriangles.push_back(p) : rightTriangles.push_back(p);
			break;
		case Axis::Y:
			median < p->bbox.getMidpoint().y ? leftTriangles.push_back(p) : rightTriangles.push_back(p);
			break;
		case Axis::Z:
			median < p->bbox.getMidpoint().z ? leftTriangles.push_back(p) : rightTriangles.push_back(p);
			break;
		}
	}
	if (leftTriangles.size() == 0) {
		node->geometries = rightTriangles;
	} else if (rightTriangles.size() == 0) {
		node->geometries = leftTriangles;
	} else {
		node->left = KDNode::build(leftTriangles, depth + 1, maxDepth);
		node->right = KDNode::build(rightTriangles, depth + 1, maxDepth);
	}
	return node;
}
