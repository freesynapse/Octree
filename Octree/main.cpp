
#include "mainGlobals.h"


// Mouse button press ....................................................
void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		switch (state)
		{
		case GLUT_DOWN:
			break;
		case GLUT_UP:
			break;
		}
	}

} // end MouseButton

// Mouse move (button pressed) ...........................................
void MouseMove(int x, int y)
{

} // end MouseMove

// Passive mouse motion ..................................................
void MouseMovePassive(int x, int y)
{
	iMouseDeltaX = iHalfScreenX - x;
	iMouseDeltaY = iHalfScreenY - y;

	if (iMouseDeltaX || iMouseDeltaY)
		pCamera->SetTarget(pCamera->HeadingSpeed() * (float)iMouseDeltaX,
		pCamera->ElevationSpeed() * (float)iMouseDeltaY);

} // end MouseMovePassive()

// Key down ..............................................................
void KeyDown(unsigned char key, int x, int y)
{
	bKeystate[key] = true;

	if (key >= 65 || key <= 90)
		bKeystate[key + 32] = true;

	else if (key == 9)
	{
		if (!bWireframeState)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		bWireframeState = !bWireframeState;
	}

	else if (key == '+')
	{

	}
	else if (key == '-')
	{

	}


} // end KeyDown()

// Key up ................................................................
void KeyUp(unsigned char key, int x, int y)
{
	bKeystate[key] = false;

	if (key >= 65 || key <= 90)
		bKeystate[key + 32] = false;

} // end KeyUp()

// Special key down ......................................................
void KeySpecialDown(int key, int x, int y)
{
	bSpecialKeystate[key] = true;

	if (key == GLUT_KEY_F1)
	{
		if (bCullState)
			glDisable(GL_CULL_FACE);
		else
			glEnable(GL_CULL_FACE);
		
		bCullState = !bCullState;
	}
	
} // end KeySpecialDown()

// Special key up ........................................................
void KeySpecialUp(int key, int x, int y)
{
	bSpecialKeystate[key] = false;

} // end KeySpecialUp()

// Asynchronous keyboard read ............................................
void KeyboardAsync()
{
	// Movement .......................................
	if (bKeystate['w'])						{ pCamera->MoveCamera(CAMERA_MOVE_FORWARD); }
	if (bKeystate['s'])						{ pCamera->MoveCamera(CAMERA_MOVE_BACKWARDS); }
	if (bKeystate['a'])						{ pCamera->MoveCamera(CAMERA_MOVE_LEFT); }
	if (bKeystate['d'])						{ pCamera->MoveCamera(CAMERA_MOVE_RIGHT); }
	if (bKeystate[32])						{ pCamera->MoveCamera(CAMERA_MOVE_UP); }	// <space>
	if (bSpecialKeystate[GLUT_KEY_SHIFT_L])	{ pCamera->MoveCamera(CAMERA_MOVE_DOWN); }	// <lshift>

	if (bSpecialKeystate[GLUT_KEY_LEFT])
	{
		fTheta += 2.0f;
		if (fTheta > 360.0f)
			fTheta -= 360.0f;
	}
	else if (bSpecialKeystate[GLUT_KEY_RIGHT])
	{
		fTheta -= 2.0f;
		if (fTheta < 0.0f)
			fTheta += 360.0f;
	}

	// exit on <esc>
	if (bKeystate[27])
	{
		Logw("\n\n--- CALL: MAIN::KeyboardAsync(): <ESC> recieved, leaving glutMainLoop().\n\n");
		glutLeaveMainLoop();
	}

} // end KeyboardAsync()

// Release ...............................................................
void Release()
{
	if (pShader)
	{
		pShader->Release();
		delete pShader;
	}

	if (pFont)
		delete pFont;

	if (pCamera)
		delete pCamera;

	glDeleteBuffers(1, &vboNodePositions);

	CloseLogFile();


} // end Release()

// SetupGeometry .........................................................
void SetupGeometry()
{

	// Setup the vertex data for rendering of the nodes
	std::vector<Vector2d> vNodes;
	vNodes.resize(nNodes);
	srand((unsigned int)time(NULL));
	for (int i = 0; i < nNodes; i++)
		vNodes[i] = Vector2d((double)(rand() % 10000 / 100.0 - 50.0), (double)(rand() % 10000 / 100.0 - 50.0));


	// Setup the vertex array object for rendering the nodes
	GLenum res = GL_NO_ERROR;
	glGenVertexArrays(1, &vaoNodes);
	res = glGetError();
	if (res != GL_NO_ERROR)
		Logw("ERROR %d: glGenVertexArrays vaoNodes: %s\n", res, gluErrorString(res));

	glBindVertexArray(vaoNodes);


	// Setup the vertex buffer object for the nodes
	res = GL_NO_ERROR;
	glGenBuffers(1, &vboNodePositions);
	res = glGetError();
	if (res != GL_NO_ERROR)
		Logw("ERROR %d: glGenBuffers vboNodePositions: %s\n", res, gluErrorString(res));

	glBindBuffer(GL_ARRAY_BUFFER, vboNodePositions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2d) * nNodes, &vNodes[0], GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(attributePositions);
	glVertexAttribPointer(attributePositions, 2, GL_DOUBLE, GL_FALSE, 0, BUFFER_OFFSET(0));

	// Unbind the vertex array object
	glDisableVertexAttribArray(attributePositions);
	glBindVertexArray(0);


	// Setup the vertex array obejct for rendering the lines
	res = GL_NO_ERROR;
	glGenVertexArrays(1, &vaoLines);
	res = glGetError();
	if (res != GL_NO_ERROR)
		Logw("ERROR %d: glGenVertexArrays vaoLines: %s\n", res, gluErrorString(res));

	glBindVertexArray(vaoLines);


	// Setup vertex buffer object for line rendering
	res = GL_NO_ERROR;
	glGenBuffers(1, &vboLines);
	res = glGetError();
	if (res != GL_NO_ERROR)
		Logw("ERROR %d: glGenBuffers vboLines: %s\n", res, gluErrorString(res));

	glBindBuffer(GL_ARRAY_BUFFER, vboLines);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2d) * nLines * 2, NULL, GL_STATIC_DRAW);

	glEnableVertexAttribArray(attributePositions);
	glVertexAttribPointer(attributePositions, 2, GL_DOUBLE, GL_FALSE, 0, BUFFER_OFFSET(0));

	// Unbind the vertex array object
	glDisableVertexAttribArray(attributePositions);
	glBindVertexArray(0);

	
} // end SetupGeometry()

// Update ................................................................
void Update()
{
	static int t0 = 0, t1;

	t1 = glutGet(GLUT_ELAPSED_TIME);
	if (t1 - t0 > 1000)
	{
		dFPS = frame * 1000.0f / (t1 - t0);
		t0 = t1;
		frame = 0;
	}

	glutWarpPointer(iHalfScreenX, iHalfScreenY);

	KeyboardAsync();

	Render();

	sg_lFrame++;
	frame++;

	Sleep(1);

} // end Update()

// Render ................................................................
void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 3D RENDERING ////////////////////////////////////////////

	// use main rendering program
	glUseProgram(main_program);

	// set the camera
	pCamera->SetHeading(270.0f);
	pCamera->SetElevation(90.0f);
	pCamera->Update();
	

	// SHADER UNIFORMS //

	// Model matrix
	mModelMatrix.Identity();
	qRotation = AxisAngleToQuaternion(fTheta, vRotationAxis);
	mModelMatrix.QuaternionRotation(qRotation);
	glUniformMatrix4fv(uniformModelMatrix, 1, GL_TRUE, (const GLfloat *)&mModelMatrix);

	// View matrix
	mViewMatrix = pCamera->ViewMatrix();
	glUniformMatrix4fv(uniformViewMatrix, 1, GL_TRUE, (const GLfloat *)&mViewMatrix);

	// Projection matrix
	mProjectionMatrix = pCamera->ProjectionMatrix();
	glUniformMatrix4fv(uniformProjectionMatrix, 1, GL_TRUE, (const GLfloat *)&mProjectionMatrix);
	
	// Set color of nodes
	glUniform3fv(uniformColor, 1, (const GLfloat *)&rgbNodeColor);


	// RENDER GEOMETRY //

	// Render the nodes
	glBindVertexArray(vaoNodes);
	glEnableVertexAttribArray(attributePositions);

	glPointSize(2.0);
	glDrawArrays(GL_POINTS, 0, nNodes);

	// Render AABB lines
	glUniform3fv(uniformColor, 1, (const GLfloat *)&Vector3f(0.0f, 0.0f, 0.0f));

	glBindVertexArray(vaoLines);
	glEnableVertexAttribArray(attributePositions);

	glDrawArrays(GL_LINES, 0, nLines * 2);
	
	
	// 2D RENDERING ////////////////////////////////////////////
	float y = 0.0f;
	static float font_height = (float)pFont->FontHeight();

	// Enable blending for texture transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUseProgram(pFont->fontProgramID());

	// current fps
	pFont->RenderString_ss(2, y += font_height, "FPS: %.2f", dFPS);
	pFont->RenderString_ss(2, y += font_height, "points: %d", nNodes);
	pFont->RenderString_ss(2, y += font_height, "rotation angle: %.0f deg", fTheta);

	pFont->RenderString_ss(2, y += font_height * 2, "Tree levels: %d", nTreeLevels);

	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (bWireframeState)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	

	// swap!
	glutSwapBuffers();


} // end Render()

// Main ..................................................................
int main(int argc, char *argv[])
{
	// initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(APPLICATION_NAME);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	// GLUT callbacks1
	glutKeyboardFunc(KeyDown);
	glutKeyboardUpFunc(KeyUp);
	glutSpecialFunc(KeySpecialDown);
	glutSpecialUpFunc(KeySpecialUp);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMove);
	glutPassiveMotionFunc(MouseMovePassive);
	glutCloseFunc(Release);
	glutIdleFunc(Update);
	glutDisplayFunc(Render);

	// glew
	glewExperimental = GL_TRUE;
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		Logw("ERROR: Could not initialize GLEW: %d\n", glewGetErrorString(res));
		return (EXIT_FAILURE);
	}

	// openGL settings
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	//glEnable(GL_ALPHA_TEST);
	//glAlphaFunc(GL_GREATER, 0.5);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 1.0f);
	glutSetCursor(GLUT_CURSOR_NONE);
	//glutSetCursor(GLUT_CURSOR_CROSSHAIR);

	// log it!
	OpenLogFile("./log.txt");

	// Load shaders
	pShader = new c_ShaderObject();
	main_program = pShader->CreateShaderProgram("./GLSL/main_vertex.glsl",
												"./GLSL/main_fragment.glsl");

	glUseProgram(main_program);
	
	attributePositions		= glGetAttribLocation(main_program, "a_vPositions");
	uniformModelMatrix		= glGetUniformLocation(main_program, "u_mModel");
	uniformViewMatrix		= glGetUniformLocation(main_program, "u_mView");
	uniformProjectionMatrix = glGetUniformLocation(main_program, "u_mProjection");
	uniformColor			= glGetUniformLocation(main_program, "u_vRenderColor");

	if (attributePositions		== -1 ||
		uniformModelMatrix		== -1 ||
		uniformViewMatrix		== -1 ||
		uniformProjectionMatrix == -1 ||
		uniformColor			== -1)
	{
		Logw("ERROR: Could not bind attribute/uniform locations:\n");
		Logw("\tattributePositions = %d\n\tuniformModelMatrix = %d\n\tuniformViewMatrix = %d\n\tuniformProjectionMatrix = %d\n\tuniformColor = %d\n",
			attributePositions, uniformModelMatrix, uniformViewMatrix, uniformProjectionMatrix, uniformColor);
		return (EXIT_FAILURE);
	}

	// initialize font atlas 
	pFont = new c_Font("./fonts/FreeSans.ttf", 14, FONT_CREATE_SHADERS);
	pFont->SetColor(&rgba4f(0.0f, 0.0f, 0.0f, 1.0f));

	// initialise camera
	pCamera = new c_Camera(main_program);
	pCamera->PerspectiveProjection(FOV,
								   (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
								   0.1f,
								   1000.0f);

	pCamera->SetCamera(Vector3f(-13.0f, 0.0f, -90.0f),
					   Vector3f(0.0f, 0.0f, 1.0f),
					   Vector3f(0.0f, 1.0f, 0.0f));

	pCamera->SetHeading(270.0f);
	pCamera->SetElevation(90.0f);
	pCamera->SetHeadingSpeed(0.2f);
	pCamera->SetElevationSpeed(0.2f * (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
	pCamera->SetMoveSpeed(1.0f);
	pCamera->Update();

	
	// setup geometry
	SetupGeometry();


	// Let's start the show!
	glutMainLoop();

	return (EXIT_SUCCESS);
}