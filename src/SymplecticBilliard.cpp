#include "SymplecticBilliard.h"
#include "shape.h"
vec2_d SymplecticBilliardMap(Poly polygon0, double t0, Poly polygon1, double t1){
		if (!(polygon0.closed && polygon1.closed) || abs(t1 - int(t1)) <  10e-5) {
		return {t0, t1}; // Polygon has to be closed
	}
	vec2_d x = polygon0.ParamEdgeRatio(t0);
	vec2_d dirX = polygon0.directions_d[int(floorf(t0))];
	vec2_d dirY = polygon1.directions_d[int(floorf(t1))];
	float signf = det(dirX, dirY);
	if (abs(signf) <  10e-5) return {t0, t1};
	vec2_d inwardDirY = sgn(signf) * dirY;
	if (sgn(signf) == 0) std::cout << "WARNING: the inward-direction became zero" << std::endl;
	float t2 = firstIntersection(polygon0, x, inwardDirY);
	return {t1, t2};
}

SymplecticBilliardSystem::SymplecticBilliardSystem()
{
	polygon0 = Poly();
	polygon1 = Poly();
	polygon0.color = vec3(0.0,1.0,1.0);
	polygon0.lineWidth = 5;
	float xOffset = 10.0;
	polygon1.AddVertex(vec2_d(0.0 - xOffset, 0.0));
	polygon1.AddVertex(vec2_d(5.0 - xOffset, 0.0));
	polygon1.AddVertex(vec2_d(0.0 - xOffset, 5.0));
	polygon1.color = vec3(0.0,1.0,1.0);
	polygon1.lineWidth = 5;
	polygon1.AddVertex(vec2_d(0.0 + xOffset, 0.0));
	polygon1.AddVertex(vec2_d(5.0 + xOffset, 0.0));
	polygon1.AddVertex(vec2_d(0.0 + xOffset, 5.0));
	x0.AddVertex(vec2_d());
	x1.AddVertex(vec2_d());
    x0.projectOntoPolygon(polygon0);
    x1.projectOntoPolygon(polygon1);
	t0 = inverseParamPolygon(x0.getPos(), polygon0);
	t1 = inverseParamPolygon(x1.getPos(), polygon1);
	evenTraj.AddVertex(x0.getPos());
	oddTraj.AddVertex(x1.getPos());
	evenTraj.color = vec3(0.8,0.0,0.0);
	oddTraj.color = vec3(0.0,0.8,0.0);

}

SymplecticBilliardSystem::SymplecticBilliardSystem(double t_0, double t_1)
{
	polygon0 = Poly();
	polygon1 = Poly();
	polygon0.color = vec3(0.0,1.0,1.0);
	polygon0.lineWidth = 5;
	polygon0.AddVertex(vec2_d(0.0, 0.0));
	polygon0.AddVertex(vec2_d(5.0, 0.0));
	polygon0.AddVertex(vec2_d(0.0, 5.0));
	polygon1.color = vec3(0.0,1.0,1.0);
	polygon1.lineWidth = 5;
	float xOffset = 10.0;
	polygon1.AddVertex(vec2_d(0.0 + xOffset, 0.0));
	polygon1.AddVertex(vec2_d(5.0 + xOffset, 0.0));
	polygon1.AddVertex(vec2_d(0.0 + xOffset, 5.0));
	t_trajectory = {t_0,t_1};
	t0 = t_0;
	t1 = t_1;
	// tx = t_0;
	// ty = t_1;
	x0.AddVertex(polygon0.ParamEdgeRatio(t0));
	x1.AddVertex(polygon1.ParamEdgeRatio(t1));
    x0.projectOntoPolygon(polygon0);
    x1.projectOntoPolygon(polygon1);
	evenTraj.AddVertex(x0.getPos());
	oddTraj.AddVertex(x1.getPos());
	evenTraj.color = vec3(0.8,0.0,0.0);
	oddTraj.color = vec3(0.0,0.8,0.0);
}

void SymplecticBilliardSystem::Create()
{
	x0.Create();
    x1.Create();
    polygon0.Create();
    polygon1.Create();
    evenTraj.Create();
    oddTraj.Create();
}

void SymplecticBilliardSystem::reset()
{
	evenTraj.Clear();
	oddTraj.Clear();
	x0.clear();
	x1.clear();
	x0.AddVertex(polygon0.ParamEdgeRatio(t0));
	x1.AddVertex(polygon1.ParamEdgeRatio(t1));
	evenTraj.AddVertex(x0.getPos());
	oddTraj.AddVertex(x1.getPos());
	t_trajectory = {t0, t1};
	// polygon0And2AreClosed = polygon0.closed && polygon1.closed;
}

// void SymplecticBilliardSystem::setInitialValues(float t_0, float t_1)
// {
// 	t0 = t_0;
// 	t1 = t_1;
// 	x0.vertexData.push_back(polygon0.ParamEdgeRatio(t0));
// 	x1.vertexData.push_back(polygon1.ParamEdgeRatio(t1));
//     x0.projectOntoPolygon(polygon0);
//     x1.projectOntoPolygon(polygon1);
// }

void SymplecticBilliardSystem::iterateSymplecticBilliards(int batch, int nIter)
{
	int i = 0;

	while (evenTraj.vertexData.size()*2 < nIter && i < batch)
	{
			vec2_d tytz = SymplecticBilliardMap(polygon0, t_trajectory[t_trajectory.size() - 2], polygon1, t_trajectory[t_trajectory.size() - 1]);
			t_trajectory.push_back(tytz.y);
			evenTraj.AddVertex(polygon0.ParamEdgeRatio(tytz.y));
			tytz = SymplecticBilliardMap(polygon1,t_trajectory[t_trajectory.size() - 2], polygon0, t_trajectory[t_trajectory.size() - 1]);
			t_trajectory.push_back(tytz.y);
			oddTraj.AddVertex(polygon1.ParamEdgeRatio(tytz.y));
			i++;
	}
}

void SymplecticBilliardSystem::updateCoords(vec2_d mouse, GLFWwindow *window)
{
}

// bool SymplecticBilliardSystem::polygon0And2Closed()
// {
//     return polygon0And2AreClosed;
// }

void SymplecticBilliardSystem::closepolygon0()
{
    polygon0.ClosePolygon();
}

void SymplecticBilliardSystem::closepolygon1()
{
    polygon0.ClosePolygon();
}

void SymplecticBilliardSystem::closepolygon0And2()
{
   this->closepolygon0();
   this->closepolygon1();
}

void SymplecticBilliardSystem::ClearPolygon0()
{
	polygon0.Clear();
	polygon0.AddVertex(vec2_d(0.0, 0.0));
	polygon0.AddVertex(vec2_d(5.0, 0.0));
	polygon0.AddVertex(vec2_d(0.0, 5.0));
	polygon0.closed = false;
	reset();

}

void SymplecticBilliardSystem::ClearPolygon1()
{
	polygon1.Clear();
	float xOffset = 10.0;
	polygon1.AddVertex(vec2_d(0.0 + xOffset, 0.0));
	polygon1.AddVertex(vec2_d(5.0 + xOffset, 0.0));
	polygon1.AddVertex(vec2_d(0.0 + xOffset, 5.0));
	polygon1.closed = false;
	reset();
}

void SymplecticBilliardSystem::copyPolygon0OntoPolygon1()
{
	polygon1 = polygon0;
}

void SymplecticBilliardSystem::copyPolygon1OntoPolygon0()
{
	polygon0 = polygon1;
}

void SymplecticBilliardSystem::editVertexPositionPolygons(GLFWwindow *window, vec2_d &pos)
{
	IndexDistPair p1 =  polygon0.computeClosestIndexDistance(window, pos);
	IndexDistPair p2 =  polygon1.computeClosestIndexDistance(window, pos);
	if(p1.dist < p2.dist) polygon0.onMouse(window, pos);
	else polygon1.onMouse(window, pos);;

}

void SymplecticBilliardSystem::translatepolygon0(vec2_d &deltaPos)
{
	polygon0.translateBy(deltaPos);
}

void SymplecticBilliardSystem::translatepolygon1(vec2_d &deltaPos)
{
	polygon1.translateBy(deltaPos);
}

void SymplecticBilliardSystem::translatepolygons(vec2_d &mouse, vec2_d &deltaPos)
{
	if(polygon0.distance(mouse) < polygon1.distance(mouse)){
		translatepolygon0(deltaPos);
	} else {
		translatepolygon1(deltaPos);
	}
}

void SymplecticBilliardSystem::centerPolygonsAt(vec2_d mouse)
{
	if(polygon0.distance(mouse) < polygon1.distance(mouse)){
		polygon0.center();
		polygon0.translateBy(mouse);
	} else {
		polygon1.center();
		polygon1.translateBy(mouse);
	}
}

void SymplecticBilliardSystem::editVertexPositionpolygon0(GLFWwindow *window, vec2_d &pos)
{
	polygon0.onMouse(window, pos);
}

void SymplecticBilliardSystem::editVertexPositionpolygon1(GLFWwindow *window, vec2_d& pos)
{
	polygon1.onMouse(window,pos);
}

void SymplecticBilliardSystem::editVertexPositionX0(GLFWwindow *window, vec2_d& pos)
{
	x0.dragDropTo(window, pos);
	x0.projectOntoPolygon(polygon0);
	t0 = inverseParamPolygon(x0.getPos(), polygon0);

}

void SymplecticBilliardSystem::editVertexPositionX1(GLFWwindow *window, vec2_d& pos)
{
	x1.dragDropTo(window, pos);
	x1.projectOntoPolygon(polygon1);
	t1 = inverseParamPolygon(x1.getPos(), polygon1);
}

void SymplecticBilliardSystem::editVertexPositionX0X1(GLFWwindow *window, vec2_d& pos)
{
	if( length(pos - x1.getPos()) < length(pos-x0.getPos())){
		x1.dragDropTo(window, pos);
		x1.projectOntoPolygon(polygon1);
		t1 = inverseParamPolygon(x1.getPos(), polygon1);
	} else {
		x0.dragDropTo(window, pos);
		x0.projectOntoPolygon(polygon0);
		t0 = inverseParamPolygon(x0.getPos(), polygon0);		
	}
}

void SymplecticBilliardSystem::snapToGridpolygon0()
{
	polygon0.roundData();
	// this -> reset(); // This would stop the system after one batch if the snap_to_grid button is pressed...
}

void SymplecticBilliardSystem::snapToGridpolygon1()
{
	polygon1.roundData();
	// this -> reset(); // This would stop the system after one batch if the snap_to_grid button is pressed...
}

void SymplecticBilliardSystem::snapToGrid()
{
	polygon0.roundData();
	polygon1.roundData();
}

void SymplecticBilliardSystem::drawTrajectories(Shader &shaderProgram)
{
	evenTraj.Draw(shaderProgram, polygon1Color);
	oddTraj.Draw(shaderProgram, polygon0Color);
}

// void SymplecticBilliardSystem::drawPolygons(Shader &shaderProgram)
// {
// 	polygon0.Draw(shaderProgram);
// 	polygon1.Draw(shaderProgram);
// }

void SymplecticBilliardSystem::drawPolygons(Shader &shaderProgram)
{
	polygon0.Draw(shaderProgram,  polygon0Color);
	polygon1.Draw(shaderProgram, polygon1Color);
}

void SymplecticBilliardSystem::drawInitialValues(Shader &shaderProgram)
{
	x0.Draw(shaderProgram, polygon1Color);
	x1.Draw(shaderProgram, polygon0Color);
}
