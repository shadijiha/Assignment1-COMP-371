#pragma once
#include <vector>

#include "Cube.h"
#include "Renderable.h"


class Grid : public Renderable
{
public:
	Grid(uint32_t count);
	virtual ~Grid()	{}

	void onCreate() override;
	void onUpdate(float dt) override;
	void onDestroyed() override;

	void regenerate(uint32_t count);

private:
	std::vector<Cube> elements;
};

