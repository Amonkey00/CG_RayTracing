#pragma once

#include <algorithm>

#include "rtweekend.h"

#include "hittable.h"
#include "hittable_list.h"

bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis) {
	aabb box_a;
	aabb box_b;
	if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b)) {
		std::cerr << "No bounding box in bvh_node constructor.\n";
	}
	return box_a.min().e[axis] < box_b.min().e[axis];
}

bool box_x_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
	return box_compare(a, b, 0);
}

bool box_y_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
	return box_compare(a, b, 1);
}

bool box_z_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
	return box_compare(a, b, 2);
}

class bvh_node : public hittable {
public:
	bvh_node();

	bvh_node(const hittable_list& list, double time0, double time1)
		: bvh_node(list.objects, 0, list.objects.size(), time0, time1) 
	{}

	bvh_node(
		const std::vector<shared_ptr<hittable>>& src_objects,
		size_t start, size_t end, double time0, double time1);

	virtual bool hit(
		const ray& r, double t_min, double t_max, hit_record& rec) const override;

	virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;

public:
	shared_ptr<hittable> left;
	shared_ptr<hittable> right;
	aabb box;
};



//构建bvh树
bvh_node::bvh_node(
	const std::vector<shared_ptr<hittable>>& src_objects,
	size_t start, size_t end, double time0, double time1) 
{
	//为src_objects创建一个副本方便后面修改，不影响src_objects
	auto objects = src_objects;
	
	int axis = random_int(0, 2);
	auto comparator = (axis == 0) ? box_x_compare
					: (axis == 1) ? box_y_compare
					: box_z_compare;
	
	size_t object_span = end - start;

	if (object_span == 1) {//仅有一个物体，复制一个，避免空指针判断
		left = right = objects[start];
	}
	else if(object_span==2){//仅有两个
		//跟据随机方向的大小，来决定左右子节点
		if (comparator(objects[start], objects[start + 1])) {
			left = objects[start];
			right = objects[start + 1];
		}
		else {
			left = objects[start + 1];
			right = objects[start];
		}
	}
	else {//三个及以上进行递归
		std::sort(objects.begin() + start, objects.begin() + end, comparator);
		auto mid = (start + end) >> 1;
		left = make_shared<bvh_node>(objects, start, mid, time0, time1);
		right = make_shared<bvh_node>(objects, mid, end, time0, time1);
	}

	//从底层递归上来的left,right，进行判别
	aabb box_left, box_right;
	if (!left->bounding_box(time0, time1, box_left)
		|| !right->bounding_box(time0, time1, box_right)) {
		std::cerr << "No bounding box in bvh_node constructor.\n";
	}

	box = surrounding_box(box_left, box_right);
}

bool bvh_node::hit(const ray& r, double t_min, double t_max, hit_record& rec)const {
	if (!box.hit(r, t_min, t_max))
		return false;

	bool hit_left = left->hit(r, t_min, t_max,rec);
	//如果左边击中目标，更新t_max，因为如果已经hit到前面的，hit点理应无法更远
	bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);

	return hit_left || hit_right;
}

bool bvh_node::bounding_box(double time0, double time1, aabb& output_box) const {
	output_box = box;
	return true;
}
