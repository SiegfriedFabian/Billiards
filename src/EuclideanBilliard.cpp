#include "EuclideanBilliard.h"


lightSource::lightSource(int n) {
    
}

EuclideanBilliardSystem::EuclideanBilliardSystem()
{
}

EuclideanBilliardSystem::EuclideanBilliardSystem(vec2_d pos, vec2_d direction)
{
}

void EuclideanBilliardSystem::Create()
{
}

void EuclideanBilliardSystem::reset() {
    
}

void EuclideanBilliardSystem::iterateEuclideanBilliards(int batch, int nIter) {
    for (auto& p : sources.vertexData)
    {
        
    }
    
}

void EuclideanBilliardSystem::updateCoords(vec2_d mouse, GLFWwindow* window) {
    
}

void EuclideanBilliardSystem::translatepolygon(vec2_d &deltaPos) {
    
}

void EuclideanBilliardSystem::centerPolygonAt(vec2_d mouse) {
    
}



void EuclideanBilliardSystem::closepolygon() {
    
}

void EuclideanBilliardSystem::clearPolygon()
{
}

void EuclideanBilliardSystem::editVertexPositionPolygon(GLFWwindow *window, vec2_d& pos) {
    
}

void EuclideanBilliardSystem::editVertexPositionX0(GLFWwindow *window, vec2_d& pos) {
    
}

void EuclideanBilliardSystem::editVertexPositionX1(GLFWwindow *window, vec2_d& pos) {
    
}


void EuclideanBilliardSystem::snapToGrid() {
    
}

void EuclideanBilliardSystem::drawPolygon(Shader &shaderProgram)
{
}

void EuclideanBilliardSystem::drawPolygon(Shader &shaderProgram, vec3 &p0Color, vec3 &p1Color)
{
}

void EuclideanBilliardSystem::drawInitialValues(Shader& shaderProgram) {
    
}

void EuclideanBilliardSystem::drawTrajectories(Shader& shaderProgram) {
    
}

std::vector<vec2_d> EuclideanBilliardSystem::getStartPoints() {
    return std::vector<vec2_d>({vec2_d()});
}


void EuclideanBilliardSystem::resetTrajectories() {
    
}

vec2_d EuclideanBilliardMap(Poly polygon0, double t0, Poly polygon1, double t1) {
    return vec2_d();
}
