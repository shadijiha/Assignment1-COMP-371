#pragma once
#include <vector>

#include "Cube.h"
#include "GameObject.h"


class Grid : public GameObject
{
public:
	Grid(uint32_t count);
	virtual ~Grid()	{}

	void onCreate() override;
	void onUpdate(float dt) override;
	void onDestroyed() override;

	void regenerate(uint32_t count);

	uint32_t getCount() const { return count; }

private:
	std::vector<Cube> elements;
	uint32_t count;
};

