#pragma once

#include <glm.hpp>

//2x2 uniform grid
const glm::vec2 grid2x2[4] = {
	glm::vec2(0.0f, 0.0f),
	glm::vec2(1.0f, 0.0f),
	glm::vec2(1.0f, 1.0f),
	glm::vec2(0.0f, 1.0f)
};

//4x4 uniform grid
const glm::vec2 grid4x4[16] = {
	glm::vec2(0.0f,        0.0f),
	glm::vec2(0.33333333f, 0.0f),
	glm::vec2(0.66666667f, 0.0f),
	glm::vec2(1.0f,        0.0f),
	glm::vec2(0.0f,        0.33333333f),
	glm::vec2(0.33333333f, 0.33333333f),
	glm::vec2(0.66666667f, 0.33333333f),
	glm::vec2(1.0f,        0.33333333f),
	glm::vec2(0.0f,        0.66666667f),
	glm::vec2(0.33333333f, 0.66666667f),
	glm::vec2(0.66666667f, 0.66666667f),
	glm::vec2(1.0f,        0.66666667f),
	glm::vec2(0.0f,        1.0f),
	glm::vec2(0.33333333f, 1.0f),
	glm::vec2(0.66666667f, 1.0f),
	glm::vec2(1.0f,        1.0f)
};

//16 points of the Halton sequence with bases 2 and 3
const glm::vec2 halton16[16] = {
	glm::vec2(0.50000f, 0.33333333f),
	glm::vec2(0.25000f, 0.66666667f),
	glm::vec2(0.75000f, 0.11111111f),
	glm::vec2(0.12500f, 0.44444444f),
	glm::vec2(0.62500f, 0.77777778f),
	glm::vec2(0.37500f, 0.22222222f),
	glm::vec2(0.87500f, 0.55555556f),
	glm::vec2(0.06250f, 0.88888889f),
	glm::vec2(0.56250f, 0.03703704f),
	glm::vec2(0.31250f, 0.37037037f),
	glm::vec2(0.81250f, 0.70370370f),
	glm::vec2(0.18750f, 0.14814815f),
	glm::vec2(0.68750f, 0.48148148f),
	glm::vec2(0.43750f, 0.81481481f),
	glm::vec2(0.93750f, 0.25925926f),
	glm::vec2(0.03125f, 0.59259259f)
};

//16 points of blue noise sequence
const glm::vec2 blueNoise16[16] = {
	glm::vec2(1.500000f, 0.593750f),
	glm::vec2(1.218750f, 1.375000f),
	glm::vec2(1.687500f, 1.906250f),
	glm::vec2(0.375000f, 0.843750f),
	glm::vec2(1.125000f, 1.875000f),
	glm::vec2(0.718750f, 1.656250f),
	glm::vec2(1.937500f, 0.718750f),
	glm::vec2(0.656250f, 0.125000f),
	glm::vec2(0.906250f, 0.937500f),
	glm::vec2(1.656250f, 1.437500f),
	glm::vec2(0.500000f, 1.281250f),
	glm::vec2(0.218750f, 0.062500f),
	glm::vec2(1.843750f, 0.312500f),
	glm::vec2(1.093750f, 0.562500f),
	glm::vec2(0.062500f, 1.218750f),
	glm::vec2(0.281250f, 1.656250f), 
};