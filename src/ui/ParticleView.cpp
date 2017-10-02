#include "ParticleView.h"

#include <QMouseEvent>

ParticleView::ParticleView(QWidget *parent)
	: QOpenGLWidget(parent),
	  model(0),
	  angularSpeed(0)
{
}

ParticleView::~ParticleView()
{
	// Make sure the context is current when deleting the texture
	// and the buffers.
	makeCurrent();
	delete model;
	doneCurrent();
}

void ParticleView::initializeGL()
{
	initializeOpenGLFunctions();
	glClearColor(0, 0, 0, 1);

	initShaders();
	model = new ParticleModel();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void ParticleView::resizeGL(int w, int h)
{
	// Calculate aspect ratio
	qreal aspect = qreal(w) / qreal(h ? h : 1);

	// Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
	const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;

	// Reset projection
	projection.setToIdentity();

	// Set perspective projection
	projection.perspective(fov, aspect, zNear, zFar);
}

void ParticleView::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	QMatrix4x4 matrix;
	matrix.translate(0.0, 0.0, -5.0);
	matrix.rotate(rotation);
	QMatrix4x4 trans = projection * matrix;
	program.setUniformValue("mvp_matrix", trans);
	model->Draw(&program);
//	glBegin(GL_POLYGON);
//	qglColor(Qt::red);
//	glVertex2i(0, 0);
//	glVertex2i(50, 50);
//	glVertex2i(100, 0);
//	glVertex2i(50, 100);
//	glEnd();
//	glLineWidth(5);
//	glBegin(GL_LINE_LOOP);
//	qglColor(Qt::green);
//	glVertex2i(0, 0);
//	glVertex2i(50, 50);
//	glVertex2i(100, 0);
//	glVertex2i(50, 100);
	//	glEnd();
}

void ParticleView::mousePressEvent(QMouseEvent *e)
{
	// Save mouse press position
	mousePressPosition = QVector2D(e->localPos());
}

void ParticleView::mouseReleaseEvent(QMouseEvent *e)
{
	// Mouse release position - mouse press position
	QVector2D diff = QVector2D(e->localPos()) - mousePressPosition;

	// Rotation axis is perpendicular to the mouse position difference
	// vector
	QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();

	// Accelerate angular speed relative to the length of the mouse sweep
	qreal acc = diff.length() / 100.0;

	// Calculate new rotation axis as weighted sum
	rotationAxis = (rotationAxis * angularSpeed + n * acc).normalized();

	// Increase angular speed
	angularSpeed += acc;
}

void ParticleView::initShaders()
{
	// Compile vertex shader
	if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
		close();

	// Compile fragment shader
	if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
		close();

	// Link shader pipeline
	if (!program.link())
		close();

	// Bind shader pipeline for use
	if (!program.bind())
		close();
}
