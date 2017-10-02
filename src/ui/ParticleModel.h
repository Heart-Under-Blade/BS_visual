#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLBuffer>

class QOpenGLShaderProgram;

class ParticleModel : protected QOpenGLFunctions
{
public:
	ParticleModel();
	virtual ~ParticleModel();

	void Draw(QOpenGLShaderProgram *program);

private:
	void Init();

	QOpenGLBuffer arrayBuf;
	QOpenGLBuffer indexBuf;
};
