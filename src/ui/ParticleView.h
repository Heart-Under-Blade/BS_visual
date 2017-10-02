#pragma once

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>

#include "ParticleModel.h"

class ParticleView : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT
public:
	ParticleView(QWidget *parent = 0);
	~ParticleView();

	// QGLWidget interface
protected:
	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;
	void mousePressEvent(QMouseEvent *e) override;
	void mouseReleaseEvent(QMouseEvent *e) override;
	void initShaders();

private:
	QOpenGLShaderProgram program;
	ParticleModel *model;
	QMatrix4x4 projection;
	QVector2D mousePressPosition;
	QVector3D rotationAxis;
	qreal angularSpeed;
	QQuaternion rotation;
};
