//chunk world[10][10];

//bool highlighted;
//glm::vec3 highPos;

/*int DrawGLScene(GLvoid)                             // Here's Where We Do All The Drawing
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);         // Clear The Screen And The Depth Buffer
	glLoadIdentity();                           // Reset The Current Modelview Matrix

	static float abc = 0;
	abc += 0.01f;

	glRotatef(ver, 1, 0, 0);
	glRotatef(hor, 0, 1, 0);
	glTranslatef(-position.x, -position.y, -position.z);

	glBindTexture(GL_TEXTURE_2D, texture);
	glUseProgram(shaderProgram); 

	glPushMatrix();
	glm::mat4 matrix, matrixold, matrixmv, matrixp;
	glTranslatef(highPos.x, highPos.y, highPos.z);
	glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(matrixmv));
	glGetFloatv(GL_PROJECTION_MATRIX, glm::value_ptr(matrixp));

	matrixold = matrixp * matrixmv;
	glPopMatrix();

	for (int i = 0; i < 100; i++)
	{
		glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(matrixmv));
		glGetFloatv(GL_PROJECTION_MATRIX, glm::value_ptr(matrixp));

		matrix = matrixp * matrixmv;
		glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, glm::value_ptr(matrix));


		chunk& current = world[i / 10][i % 10];
		glUniform3fv(highLoc, 1, glm::value_ptr(current.highlighted));

		glBindVertexArray(current.vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current.ebo);

		glDrawElements(GL_TRIANGLES, current.icount, GL_UNSIGNED_INT, 0);

		glTranslatef(0, 0, 8);
		if (i != 0 && i % 10 == 9)
			glTranslatef(8, 0, -80);
	}

	if (highlighted)
	{
		glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, glm::value_ptr(matrixold));

		

		glBindVertexArray(CubeVAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CubeEBO);

		glDrawElements(GL_TRIANGLES, 3 * 12, GL_UNSIGNED_INT, 0);
	}
	return TRUE;
}*/

/*GLvoid KillGLWindow(GLvoid)                         // Properly Kill The Window
{
	if (fullscreen)                             // Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL, 0);                  // If So Switch Back To The Desktop
		ShowCursor(TRUE);                       // Show Mouse Pointer
	}
	if (hRC)                                // Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL, NULL))                 // Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL, L"Release Of DC And RC Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		if (!wglDeleteContext(hRC))                 // Are We Able To Delete The RC?
		{
			MessageBox(NULL, L"Release Rendering Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		hRC = NULL;                           // Set RC To NULL
	}
	if (hDC && !ReleaseDC(hWnd, hDC))                    // Are We Able To Release The DC
	{
		MessageBox(NULL, L"Release Device Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hDC = NULL;                           // Set DC To NULL
	}
	if (hWnd && !DestroyWindow(hWnd))                   // Are We Able To Destroy The Window?
	{
		MessageBox(NULL, L"Could Not Release hWnd.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;                          // Set hWnd To NULL
	}
	if (!UnregisterClass(L"OpenGL", hInstance))               // Are We Able To Unregister Class
	{
		MessageBox(NULL, L"Could Not Unregister Class.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hInstance = NULL;                         // Set hInstance To NULL
	}
}*/
/*BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint      PixelFormat;
	WNDCLASS    wc;
	DWORD       dwExStyle;                      // Window Extended Style
	DWORD       dwStyle;
	RECT WindowRect;                            // Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left = (long)0;                        // Set Left Value To 0
	WindowRect.right = (long)width;                       // Set Right Value To Requested Width
	WindowRect.top = (long)0;                         // Set Top Value To 0
	WindowRect.bottom = (long)height;
	fullscreen = fullscreenflag;

	hInstance = GetModuleHandle(NULL);            // Grab An Instance For Our Window
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;       // Redraw On Move, And Own DC For Window
	wc.lpfnWndProc = (WNDPROC)WndProc;                // WndProc Handles Messages
	wc.cbClsExtra = 0;                        // No Extra Window Data
	wc.cbWndExtra = 0;                        // No Extra Window Data
	wc.hInstance = hInstance;                    // Set The Instance
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);          // Load The Default Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);          // Load The Arrow Pointer
	wc.hbrBackground = NULL;                     // No Background Required For GL
	wc.lpszMenuName = NULL;                     // We Don't Want A Menu
	wc.lpszClassName = L"OpenGL";                 // Set The Class Name
	if (!RegisterClass(&wc))                        // Attempt To Register The Window Class
	{
		MessageBox(NULL, L"Failed To Register The Window Class.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Exit And Return FALSE
	}

	if (fullscreen)                             // Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;                   // Device Mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));       // Makes Sure Memory's Cleared
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);       // Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth = width;            // Selected Screen Width
		dmScreenSettings.dmPelsHeight = height;           // Selected Screen Height
		dmScreenSettings.dmBitsPerPel = bits;             // Selected Bits Per Pixel
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Run In A Window.
			if (MessageBox(NULL, L"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", L"NeHe GL", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			{
				fullscreen = FALSE;               // Select Windowed Mode (Fullscreen=FALSE)
			}
			else
			{   // Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL, L"Program Will Now Close.", L"ERROR", MB_OK | MB_ICONSTOP);
				return FALSE;                   // Exit And Return FALSE
			}
		}
	}
	if (fullscreen)                             // Are We Still In Fullscreen Mode?
	{
		dwExStyle = WS_EX_APPWINDOW;                  // Window Extended Style
		dwStyle = WS_POPUP;                       // Windows Style
		ShowCursor(FALSE);                      // Hide Mouse Pointer
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;           // Window Extended Style
		dwStyle = WS_OVERLAPPEDWINDOW;                    // Windows Style
	}
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);     // Adjust Window To True Requested Size

	int nTitle = MultiByteToWideChar(CP_ACP, 0, title, -1, NULL, 0);
	WCHAR* wTitle = new WCHAR[nTitle];
	MultiByteToWideChar(CP_ACP, 0, title, -1, (LPWSTR)wTitle, nTitle);

	if (!(hWnd=CreateWindowEx(  dwExStyle,              // Extended Style For The Window
		L"OpenGL",               // Class Name
		wTitle,                  // Window Title
		WS_CLIPSIBLINGS |           // Required Window Style
		WS_CLIPCHILDREN |           // Required Window Style
		dwStyle,                // Selected Window Style
		0, 0,                   // Window Position
		WindowRect.right - WindowRect.left,   // Calculate Adjusted Window Width
		WindowRect.bottom - WindowRect.top,   // Calculate Adjusted Window Height
		NULL,                   // No Parent Window
		NULL,                   // No Menu
		hInstance,              // Instance
		NULL)))                 // Don't Pass Anything To WM_CREATE
	{
		KillGLWindow();                         // Reset The Display
		MessageBox(NULL, L"Window Creation Error.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Return FALSE
	}
	static  PIXELFORMATDESCRIPTOR pfd =                  // pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),                  // Size Of This Pixel Format Descriptor
		1,                              // Version Number
		PFD_DRAW_TO_WINDOW |                        // Format Must Support Window
		PFD_SUPPORT_OPENGL |                        // Format Must Support OpenGL
		PFD_DOUBLEBUFFER,                       // Must Support Double Buffering
		PFD_TYPE_RGBA,                          // Request An RGBA Format
		bits,                               // Select Our Color Depth
		0, 0, 0, 0, 0, 0,                       // Color Bits Ignored
		0,                              // No Alpha Buffer
		0,                              // Shift Bit Ignored
		0,                              // No Accumulation Buffer
		0, 0, 0, 0,                         // Accumulation Bits Ignored
		16,                             // 16Bit Z-Buffer (Depth Buffer)
		0,                              // No Stencil Buffer
		0,                              // No Auxiliary Buffer
		PFD_MAIN_PLANE,                         // Main Drawing Layer
		0,                              // Reserved
		0, 0, 0                             // Layer Masks Ignored
	};
	if (!(hDC = GetDC(hWnd)))                         // Did We Get A Device Context?
	{
		KillGLWindow();                         // Reset The Display
		MessageBox(NULL, L"Can't Create A GL Device Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Return FALSE
	}
	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))             // Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();                         // Reset The Display
		MessageBox(NULL, L"Can't Find A Suitable PixelFormat.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Return FALSE
	}
	if (!SetPixelFormat(hDC, PixelFormat, &pfd))               // Are We Able To Set The Pixel Format?
	{
		KillGLWindow();                         // Reset The Display
		MessageBox(NULL, L"Can't Set The PixelFormat.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Return FALSE
	}
	if (!(hRC = wglCreateContext(hDC)))                   // Are We Able To Get A Rendering Context?
	{
		KillGLWindow();                         // Reset The Display
		MessageBox(NULL, L"Can't Create A GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Return FALSE
	}
	if (!wglMakeCurrent(hDC, hRC))                        // Try To Activate The Rendering Context
	{
		KillGLWindow();                         // Reset The Display
		MessageBox(NULL, L"Can't Activate The GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Return FALSE
	}
	ShowWindow(hWnd, SW_SHOW);                       // Show The Window
	SetForegroundWindow(hWnd);                      // Slightly Higher Priority
	SetFocus(hWnd);                             // Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);                       // Set Up Our Perspective GL Screen
	if (!InitGL())                              // Initialize Our Newly Created GL Window
	{
		KillGLWindow();                         // Reset The Display
		MessageBox(NULL, L"Initialization Failed.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;                           // Return FALSE
	}
	return TRUE;                                // Success
}*/
/*LRESULT CALLBACK WndProc(HWND    hWnd,                   // Handle For This Window
	UINT    uMsg,                   // Message For This Window
	WPARAM  wParam,                 // Additional Message Information
	LPARAM  lParam)                 // Additional Message Information
{
	switch (uMsg)                               // Check For Windows Messages
	{
	case WM_ACTIVATE:                       // Watch For Window Activate Message
	{
		if (!HIWORD(wParam))                    // Check Minimization State
		{
			active = TRUE;                    // Program Is Active
		}
		else
		{
			active = FALSE;                   // Program Is No Longer Active
		}

		return 0;                       // Return To The Message Loop
	}
	case WM_SYSCOMMAND:
	{
		switch (wParam)
		{
		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
			return 0;
		}
		break;
	}
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_KEYDOWN:
	{
		keys[wParam] = TRUE;
		return 0;
	}
	case WM_KEYUP:
	{
		keys[wParam] = FALSE;
		return 0;
	}
	case WM_SIZE:
	{
		ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));
		return 0;
	}
	case WM_MOUSEMOVE:
	{
		int x = (GET_X_LPARAM(lParam) - 640),
			y = (GET_Y_LPARAM(lParam) - 360);
		hor += x / 10.0f;
		ver += y / 10.0f;

		printf("Mouse x: %i, y: %i\n", x, y);

		POINT screenCenter;
		screenCenter.x = 640;
		screenCenter.y = 360;
		ClientToScreen(hWnd, &screenCenter);
		SetCursorPos(screenCenter.x, screenCenter.y);
		return 0;
	}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
*/

//void prepareCube(short);


/*
struct vertex
{
	float x, y, z;
	float u, v;
	float material; //;)
};

void pushVertex(std::vector<vertex>& vector, unsigned short vindex, glm::vec3 position, short type, bool highlighted)
{
	const float verticesc[5 * 14] = {
		   0, 0, 0, 0, 0.333333333f,
		   0, 1, 0, 0.25f, 0.333333f,
		   1, 1, 0, 0.25f, 0.66666f,
		   1, 0, 0, 0, 0.666666f,
		   0, 0, 1, 0.75f, 0.33333f,
		   0, 1, 1, 0.5f, 0.33333f,
		   1, 1, 1, 0.5f, 0.66666f,
		   1, 0, 1, 0.75, 0.66666f,
		   0, 0, 0, 1, 0.333333f,
		   1, 0, 0, 1, 0.666666f,
		   0, 0, 0, 0.25f, 0,
		   0, 0, 1, 0.5f, 0,
		   1, 0, 0, 0.25f, 1,
		   1, 0, 1, 0.5f, 1 };

	vertex newVertex;

	newVertex.x = (verticesc[5 * vindex + 0] + position.x);
	newVertex.y = (verticesc[5 * vindex + 1] + position.y);
	newVertex.z = (verticesc[5 * vindex + 2] + position.z);

	if (highlighted)
	{
		newVertex.x -= 0.001;
		newVertex.x *= 1.002;
		newVertex.y -= 0.001;
		newVertex.y *= 1.002;
		newVertex.z -= 0.001;
		newVertex.z *= 1.002;
	}

	newVertex.u = (verticesc[5 * vindex + 3]);
	newVertex.v = ((verticesc[5 * vindex + 4] + type - 1) / 3);

	newVertex.material = highlighted ? 1.0f : 0;

	vector.push_back(newVertex);
}

void pushIndex(std::vector<unsigned int>& vector, unsigned int size)
{
	//size /= 6;
	vector.push_back(size - 4);
	vector.push_back(size - 2);
	vector.push_back(size - 3);

	vector.push_back(size - 2);
	vector.push_back(size - 1);
	vector.push_back(size - 3);
}

void prepareChunk ( chunk &current )
{
	std::vector<vertex> vertices;
	std::vector<unsigned int> indices;

	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 20; y++)
		{
			for (int z = 0; z < 8; z++)
			{
				glm::vec3 pos = { x, y, z };
				short type = current.block[x][y][z];
				bool highlighted = false;
				/*if (current.highlighted.x == x &&
					current.highlighted.y == y &&
					current.highlighted.z == z)
					highlighted = true;*/
/*
				if (type) //Is solid?
				{
					if (x == 7 || (x < 7 && !current.block[x + 1][y][z]))
					{
						pushVertex(vertices, 2, pos, type, highlighted);
						pushVertex(vertices, 6, pos, type, highlighted);
						pushVertex(vertices, 12, pos, type, highlighted);
						pushVertex(vertices, 13, pos, type, highlighted);
						
						pushIndex(indices, vertices.size());
					}
					if (x == 0 || (x > 0 && !current.block[x - 1][y][z]))
					{
						pushVertex(vertices, 5, pos, type, highlighted);
						pushVertex(vertices, 1, pos, type, highlighted);
						pushVertex(vertices, 11, pos, type, highlighted);
						pushVertex(vertices, 10, pos, type, highlighted);

						pushIndex(indices, vertices.size());
					}
					if (y == 19 || (y < 19 && !current.block[x][y + 1][z]))
					{
						pushVertex(vertices, 5, pos, type, highlighted);
						pushVertex(vertices, 6, pos, type, highlighted);
						pushVertex(vertices, 1, pos, type, highlighted);
						pushVertex(vertices, 2, pos, type, highlighted);

						pushIndex(indices, vertices.size());
					}
				    if (y == 0 || (y > 0 && !current.block[x][y - 1][z]))
					{
						pushVertex(vertices, 4, pos, type, highlighted);
						pushVertex(vertices, 8, pos, type, highlighted);
						pushVertex(vertices, 7, pos, type, highlighted);
						pushVertex(vertices, 9, pos, type, highlighted);

						pushIndex(indices, vertices.size());
					}
					if (z == 7 || (z < 7 && !current.block[x][y][z + 1]))
					{
						pushVertex(vertices, 5, pos, type, highlighted);
						pushVertex(vertices, 4, pos, type, highlighted);
						pushVertex(vertices, 6, pos, type, highlighted);
						pushVertex(vertices, 7, pos, type, highlighted);

						pushIndex(indices, vertices.size());
					}
					if (z == 0 || (z > 0 && !current.block[x][y][z - 1]))
					{
						pushVertex(vertices, 0, pos, type, highlighted);
						pushVertex(vertices, 1, pos, type, highlighted);
						pushVertex(vertices, 3, pos, type, highlighted);
						pushVertex(vertices, 2, pos, type, highlighted);

						pushIndex(indices, vertices.size());
					}
				}
			}
		}
	}

	glGenVertexArrays(1, &current.vao);
	glBindVertexArray(current.vao);

	glGenBuffers(1, &current.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, current.vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &current.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);

	current.icount = indices.size();
}

void prepareCube(short type)
{
	std::vector<vertex> vertices;
	std::vector<unsigned int> indices;

	const glm::vec3 pos = glm::vec3(0);
	const bool highlighted = true;
	pushVertex(vertices, 2, pos, type, highlighted);
	pushVertex(vertices, 6, pos, type, highlighted);
	pushVertex(vertices, 12, pos, type, highlighted);
	pushVertex(vertices, 13, pos, type, highlighted);

	pushIndex(indices, vertices.size());

	pushVertex(vertices, 5, pos, type, highlighted);
	pushVertex(vertices, 1, pos, type, highlighted);
	pushVertex(vertices, 11, pos, type, highlighted);
	pushVertex(vertices, 10, pos, type, highlighted);

	pushIndex(indices, vertices.size());

	pushVertex(vertices, 5, pos, type, highlighted);
	pushVertex(vertices, 6, pos, type, highlighted);
	pushVertex(vertices, 1, pos, type, highlighted);
	pushVertex(vertices, 2, pos, type, highlighted);

	pushIndex(indices, vertices.size());

	pushVertex(vertices, 4, pos, type, highlighted);
	pushVertex(vertices, 8, pos, type, highlighted);
	pushVertex(vertices, 7, pos, type, highlighted);
	pushVertex(vertices, 9, pos, type, highlighted);

	pushIndex(indices, vertices.size());

	pushVertex(vertices, 5, pos, type, highlighted);
	pushVertex(vertices, 4, pos, type, highlighted);
	pushVertex(vertices, 6, pos, type, highlighted);
	pushVertex(vertices, 7, pos, type, highlighted);

	pushIndex(indices, vertices.size());

	pushVertex(vertices, 0, pos, type, highlighted);
	pushVertex(vertices, 1, pos, type, highlighted);
	pushVertex(vertices, 3, pos, type, highlighted);
	pushVertex(vertices, 2, pos, type, highlighted);

	pushIndex(indices, vertices.size());
	
	glGenVertexArrays(1, &CubeVAO);
	glBindVertexArray(CubeVAO);

	glGenBuffers(1, &CubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, CubeVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &CubeEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);
}
*/
/*void prepareBuffers(void)
{
	float vertices[5 * 14] = {
		0, 0, 0, 0, 0.333333333f,
		0, 1, 0, 0.25f, 0.333333f,
		1, 1, 0, 0.25f, 0.66666f,
		1, 0, 0, 0, 0.666666f,
		0, 0, 1, 0.75f, 0.33333f,
		0, 1, 1, 0.5f, 0.33333f,
		1, 1, 1, 0.5f, 0.66666f,
		1, 0, 1, 0.75, 0.66666f,
		0, 0, 0, 1, 0.333333f, 
		1, 0, 0, 1, 0.666666f,
		0, 0, 0, 0.25f, 0,
		0, 0, 1, 0.5f, 0,
		1, 0, 0, 0.25f, 1,
		1, 0, 1, 0.5f, 1};

	const unsigned int indices[3 * 12] = {
		0, 2, 1,
		0, 3, 2,
		12, 6, 2,
		12, 13, 6,
		10, 1, 5,
		10, 5, 11,
		9, 8, 4,
		9, 4, 7,
		5, 6, 7,
		5, 7, 4,
		1, 6, 5,
		1, 2, 6
	};

	for (int i = 0; i < 5 * 14; i++)
	{
		if (i % 5 == 3 && i % 5 == 4)
			continue;
		vertices[i] -= 0.001f;
		vertices[i] *= 1.002f;
	}
	
	glGenVertexArrays(1, &CubeVAO);
	glBindVertexArray(CubeVAO);

	glGenBuffers(1, &CubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, CubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &CubeEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

}*/
/*
void setupShaders(void)
{
	const char* vertexSource = R""""(
        #version 330 core
        layout (location = 0) in vec3 aPos;
		layout (location = 1) in vec2 aTexCoord;
		layout (location = 2) in float material;
		
		uniform mat4 matrix;
		//uniform vec3 highlightedPos;

		out vec2 TexCoord;		
		out float highlighted;

        void main()
        {
            gl_Position = matrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);

			highlighted = 0;
			/*if((aPos.x == highlightedPos.x || aPos.x == highlightedPos.x + 1) &&
				(aPos.y == highlightedPos.y || aPos.y == highlightedPos.y + 1) &&
				(aPos.z == highlightedPos.z || aPos.z == highlightedPos.z + 1))
				highlighted = 1;*/
			//if(material1 == 1)
				//highlighted = 1.0;
			/*if(material == 1)
				highlighted = 1.0;
			TexCoord = aTexCoord;
        }
        )"""";

	const char* fragmentSource = R""""(
        #version 330 core
		
		in vec2 TexCoord;
		in float highlighted;
        out vec4 FragColor;

		uniform sampler2D ourTexture;

        void main()
        {
			FragColor = texture(ourTexture, TexCoord);

			if(highlighted == 1.0)
				FragColor = mix(FragColor, vec4(1, 1, 1, 1), 0.3);
			FragColor.a = 1;
			//FragColor = vec4(highlighted, 0, 0, 1);
			//FragColor = vec4(TexCoord.x, TexCoord.y, 0, 1);
        } 
        )"""";

	unsigned int vertexShader, fragmentShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader); 
	
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	
	shaderProgram = glCreateProgram(); 
	
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram); 
	
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "Error linking program" << infoLog << std::endl;
	}
	glUseProgram(shaderProgram); 

	matrixLoc = glGetUniformLocation(shaderProgram, "matrix");
	glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);
	highLoc = glGetUniformLocation(shaderProgram, "highlightedPos");

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void loadTexture( void )
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); 
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 48 * 3, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	int width, height, nrChannels;
	unsigned char* data = stbi_load("grass.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		printf("Error: %i\n", glGetError());
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
		printf("Error: %i\n", glGetError());
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);
	data = stbi_load("stone.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		printf("Error: %i\n", glGetError());
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 48, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
		printf("Error: %i\n", glGetError());
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	
	stbi_image_free(data);
	data = stbi_load("mud.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		printf("Error: %i\n", glGetError());
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 48 * 2, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
		printf("Error: %i\n", glGetError());
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	stbi_image_free(data);
}
*/

/*for (int c = 0; c < 100; c++)
	{
		chunk &current = world[c / 10][c % 10];

		for (int i = 0; i < 64; i++)
		{
			int height = db::perlin<float>(((float)i / 8 + c / 10 * 8) / 30.0f, (float)(i % 8 + c % 10 * 8) / 30.0f) * 8 + 9;
			for (int h = 0; h < height; h++)
			{
				if (height - h == 1)
					current.block[i / 8][h][i % 8] = 1;
				else if(height - h <= 3)
					current.block[i / 8][h][i % 8] = 3;
				else
					current.block[i / 8][h][i % 8] = 2;
			}
		}

		prepareChunk(current);
	}*/
	//setupShaders();
	//loadTexture();
	
	//printf("Mouse x: %i, y: %i\n", x, y);

		//glm::vec3 pos;

		/*bool hit = false;
		for (int i = 0; i < 100; i++)
		{
			pos.x = floor(position.x + sin(TORAD(hor)) * cos(TORAD(ver)) * i);
			pos.y = floor(position.y + sin(TORAD(ver)) * -i);
			pos.z = floor(position.z + cos(TORAD(hor)) * cos(TORAD(ver)) * -i);

			int chunkx = pos.x / 8,
				chunkz = pos.z / 8;

			short& block = world[chunkx][chunkz].block[(int)pos.x - chunkx * 8][(int)pos.y][(int)pos.z - chunkz * 8];
			printf("Block: %i\n", block);

			if (block)
			{
				hit = true;
				if (!highlighted || highPos != pos)
				{
					highlighted = true;
					highPos = pos;
					prepareCube(world[chunkx][chunkz].block[(int)pos.x - chunkx * 8][(int)pos.y][(int)pos.z - chunkz * 8]);
					//world[chunkx][chunkz].highlighted = { (int)pos.x - chunkx * 8, (int)pos.y, (int)pos.z - chunkz * 8 };
					//block = 0;
					//prepareChunk(world[chunkx][chunkz]);
				}
				break;
			}
		}
		if (!hit)
			highlighted = false;*/

			/*glm::vec3 direction = glm::vec3(0);
			if (BETWEEN(hor, 315, 360) || BETWEEN(hor, 0, 45))
				direction.z = -1.0f;
			else if (BETWEEN(hor, 45, 135))
				direction.x = 1.0f;
			else if (BETWEEN(hor, 135, 225))
				direction.z = 1.0f;
			else if (BETWEEN(hor, 225, 315))
				direction.x = -1.0f;

			if (BETWEEN(ver, 45, 90))
				direction.y = 1.0f;
			if (BETWEEN(ver, -90, -45))
				direction.y = -1.0f;

			glm::vec3 oldpos = position,
				apos = position; //Actual position

			float horcoef, vercoef;
			if (direction.z)
				horcoef = -tan(TORAD(hor));
			else if (direction.x)
				horcoef = tan(TORAD(hor - 90.0f));

			vercoef = tan(TORAD(ver));

			bool hit = false;
			for (int i = 0; i < 100; i++)
			{
				const glm::vec3 posint = { floor(apos.x), floor(apos.y), floor(apos.z) };
				const unsigned int chunkx = (int)apos.x / 8,
					chunkz = (int)apos.z / 8;
				short& block = world[chunkx][chunkz].block[(int)posint.x - chunkx * 8][(int)posint.y][(int)posint.z - chunkz * 8];

				if (block)
				{
					hit = true;
					if (!highlighted || highPos != posint)
					{
						highlighted = true;
						highPos = posint;
						prepareCube(world[chunkx][chunkz].block[(int)posint.x - chunkx * 8][(int)posint.y][(int)posint.z - chunkz * 8]);
						//world[chunkx][chunkz].highlighted = { (int)pos.x - chunkx * 8, (int)pos.y, (int)pos.z - chunkz * 8 };
						//block = 0;
						//prepareChunk(world[chunkx][chunkz]);
					}
					break;
				}

				//Assuming dir.y = 0
				if (direction.x)
				{
					apos.z -= horcoef * (apos.x - floor(apos.x));
					apos.x = floor(apos.x);
					apos += direction;
					apos.z += horcoef * direction.x;
					apos.y -= vercoef;

					if (floor(apos.z) != floor(oldpos.z) && floor(apos.y) != floor(oldpos.y))
					{
						const glm::vec3 posint = { floor(apos.x), floor(apos.y), floor(apos.z) };
						const unsigned int chunkx = (int)apos.x / 8,
							chunkz = (int)apos.z / 8;
						short& block = world[chunkx][chunkz].block[(int)posint.x - chunkx * 8][(int)posint.y][(int)posint.z - chunkz * 8];

						if (block)
						{
							hit = true;
							if (!highlighted || highPos != posint)
							{
								highlighted = true;
								highPos = posint;
								prepareCube(world[chunkx][chunkz].block[(int)posint.x - chunkx * 8][(int)posint.y][(int)posint.z - chunkz * 8]);
							}
							break;
						}
					}
				}
				else if (direction.z)
				{
					apos.x -= horcoef * (apos.z - floor(apos.z));
					apos.z = floor(apos.z);
					apos += direction;
					apos.x += horcoef * direction.z;
					apos.y -= vercoef;

					if (floor(apos.x) != floor(oldpos.x) && floor(apos.y) != floor(oldpos.y))
					{
						const glm::vec3 posint = { floor(apos.x), floor(apos.y), floor(apos.z) };
						const unsigned int chunkx = (int)apos.x / 8,
							chunkz = (int)apos.z / 8;
						short& block = world[chunkx][chunkz].block[(int)posint.x - chunkx * 8][(int)posint.y][(int)posint.z - chunkz * 8];

						if (block)
						{
							hit = true;
							if (!highlighted || highPos != posint)
							{
								highlighted = true;
								highPos = posint;
								prepareCube(world[chunkx][chunkz].block[(int)posint.x - chunkx * 8][(int)posint.y][(int)posint.z - chunkz * 8]);
							}
							break;
						}
					}
				}
			}
			if (!hit)
				highlighted = false;*/

				/*glm::vec3 direction = glm::vec3(0);
				if (BETWEEN(hor, 315, 360) || BETWEEN(hor, 0, 45))
					direction.z = -1.0f;
				else if (BETWEEN(hor, 45, 135))
					direction.x = 1.0f;
				else if (BETWEEN(hor, 135, 225))
					direction.z = 1.0f;
				else if (BETWEEN(hor, 225, 315))
					direction.x = -1.0f;

				if (BETWEEN(ver, 45, 90))
					direction.y = 1.0f;
				if (BETWEEN(ver, -90, -45))
					direction.y = -1.0f;

				glm::vec3 oldpos = position,
					apos = position; //Actual position
				glm::vec3 posint = { floor(apos.x), floor(apos.y), floor(apos.z) };

				float horcoef, vercoef;
				if (direction.z)
					horcoef = -tan(TORAD(hor));
				else if (direction.x)
					horcoef = tan(TORAD(hor - 90.0f));

				vercoef = tan(TORAD(ver));

				bool hit = false;
				for (int i = 0; i < 100; i++)
				{
					const unsigned int chunkx = (int)apos.x / 8,
						chunkz = (int)apos.z / 8;
					short& block = world[chunkx][chunkz].block[(int)posint.x - chunkx * 8][(int)posint.y][(int)posint.z - chunkz * 8];

					if (block)
					{
						hit = true;
						if (!highlighted || highPos != posint)
						{
							highlighted = true;
							highPos = posint;
							prepareCube(world[chunkx][chunkz].block[(int)posint.x - chunkx * 8][(int)posint.y][(int)posint.z - chunkz * 8]);
							//world[chunkx][chunkz].highlighted = { (int)pos.x - chunkx * 8, (int)pos.y, (int)pos.z - chunkz * 8 };
							//block = 0;
							//prepareChunk(world[chunkx][chunkz]);
						}
						break;
					}

					//Assuming dir.y = 0
					if (direction.x)
					{
						apos.z -= horcoef * (apos.x - floor(apos.x));
						apos.x = floor(apos.x);
						apos += direction;
						apos.z += horcoef * direction.x;
						apos.y -= vercoef;

						if (floor(apos.z) != floor(oldpos.z) && floor(apos.y) != floor(oldpos.y))
						{
							const glm::vec3 posint = { floor(apos.x), floor(apos.y), floor(apos.z) };
							const unsigned int chunkx = (int)apos.x / 8,
								chunkz = (int)apos.z / 8;
							short& block = world[chunkx][chunkz].block[(int)posint.x - chunkx * 8][(int)posint.y][(int)posint.z - chunkz * 8];

							if (block)
							{
								hit = true;
								if (!highlighted || highPos != posint)
								{
									highlighted = true;
									highPos = posint;
									prepareCube(world[chunkx][chunkz].block[(int)posint.x - chunkx * 8][(int)posint.y][(int)posint.z - chunkz * 8]);
								}
								break;
							}
						}
					}
					else if (direction.z)
					{
						apos.x -= horcoef * (apos.z - floor(apos.z));
						apos.z = floor(apos.z);
						apos += direction;
						apos.x += horcoef * direction.z;
						apos.y -= vercoef;

						if (floor(apos.x) != floor(oldpos.x) && floor(apos.y) != floor(oldpos.y))
						{
							const glm::vec3 posint = { floor(apos.x), floor(apos.y), floor(apos.z) };
							const unsigned int chunkx = (int)apos.x / 8,
								chunkz = (int)apos.z / 8;
							short& block = world[chunkx][chunkz].block[(int)posint.x - chunkx * 8][(int)posint.y][(int)posint.z - chunkz * 8];

							if (block)
							{
								hit = true;
								if (!highlighted || highPos != posint)
								{
									highlighted = true;
									highPos = posint;
									prepareCube(world[chunkx][chunkz].block[(int)posint.x - chunkx * 8][(int)posint.y][(int)posint.z - chunkz * 8]);
								}
								break;
							}
						}
					}
				}
				hit = false;
				if (!hit)
					highlighted = false;*/
				
				/*if (win.GetKey('F'))
			{
				win.SetKey('F', false);
				/*glm::vec3 direction = glm::vec3(0);
				if (BETWEEN(hor, 315, 360) || BETWEEN(hor, 0, 45))
					direction.z = -1.0f;
				else if (BETWEEN(hor, 45, 135))
					direction.x = 1.0f;
				else if (BETWEEN(hor, 135, 225))
					direction.z = 1.0f;
				else if (BETWEEN(hor, 225, 315))
					direction.x = -1.0f;

				if (BETWEEN(ver, 45, 90))
					direction.y = 1.0f;
				if (BETWEEN(ver, -90, -45))
					direction.y = -1.0f;

				glm::vec3 oldpos = position,
					apos = position; //Actual position
				glm::vec3 posint = { floor(apos.x), floor(apos.y), floor(apos.z) };

				float horcoef, vercoef;
				if (direction.z)
					horcoef = -tan(TORAD(hor));
				else if (direction.x)
					horcoef = tan(TORAD(hor - 90.0f));

				vercoef = tan(TORAD(ver));

				if (direction.y)
				{
					float change = (floor(apos.y) + direction.y) - apos.y;
					apos.y = floor(apos.y) + direction.y;
					apos.z += horcoef * change;
					apos.x += horcoef * change;
				}
				else if (direction.x)
				{
					float change = (floor(apos.x) + direction.x) - apos.x;
					apos.x = floor(apos.x) + direction.x;
					apos.z += horcoef * change;
					apos.y += horcoef * change;
				}
				else if (direction.z)
				{
					float change = (floor(apos.x) + direction.x) - apos.x;
					apos.z = floor(apos.z) + direction.z;
					apos.z += horcoef * change;
					apos.y += horcoef * change;
				}

				bool hit = false;
				for (int i = 0; i < 100; i++)
				{
					const unsigned int chunkx = (int)apos.x / 8,
						chunkz = (int)apos.z / 8;
					short& block = world[chunkx][chunkz].block[(int)posint.x - chunkx * 8][(int)posint.y][(int)posint.z - chunkz * 8];

					if (!block)
					{
						block = 2;
						prepareChunk(world[chunkx][chunkz]);
					}
					//else
						//break;

					//Assuming dir.y = 0
					if (direction.x)
					{
						apos.z -= horcoef * (apos.x - floor(apos.x));
						apos.x = floor(apos.x);
						apos += direction;
						apos.z += horcoef * direction.x;
						apos.y -= vercoef;

						if (floor(apos.z) != floor(oldpos.z) || floor(apos.y) != floor(oldpos.y))
						{
							const glm::vec3 posint = { floor(apos.x), floor(apos.y), floor(apos.z) };
							const unsigned int chunkx = (int)apos.x / 8,
								chunkz = (int)apos.z / 8;
							short& block = world[chunkx][chunkz].block[(int)posint.x - chunkx * 8][(int)posint.y][(int)posint.z - chunkz * 8];

							if (!block)
							{
								block = 2;
								prepareChunk(world[chunkx][chunkz]);
							}
							//else
								//break;
						}
					}
					else if (direction.z)
					{
						apos.x -= horcoef * (apos.z - floor(apos.z));
						apos.z = floor(apos.z);
						apos += direction;
						apos.x += horcoef * direction.z;
						apos.y -= vercoef;

						if (floor(apos.x) != floor(oldpos.x) || floor(apos.y) != floor(oldpos.y))
						{
							const glm::vec3 posint = { floor(apos.x), floor(apos.y), floor(apos.z) };
							const unsigned int chunkx = (int)apos.x / 8,
								chunkz = (int)apos.z / 8;
							short& block = world[chunkx][chunkz].block[(int)posint.x - chunkx * 8][(int)posint.y][(int)posint.z - chunkz * 8];

							if (!block)
							{
								block = 2;
								prepareChunk(world[chunkx][chunkz]);
							}
							//else
								//break;
						}
					}
				}*/
				
				
				
				/*class ChunkGenerating : public Chunk
{
public:
	bool done = false;
	int generatingStatus = 0;
	glm::ivec3 position;

	void UpdateGen(int step)
	{
		if (done)
			return;

		for (unsigned int x = generatingStatus; x < generatingStatus + step; x++)
			for (unsigned int z = 0; z < Chunk::chunkSizeXZ; z++)
			{
				float perlinx = fmod((position.x * (float)Chunk::chunkSizeXZ + (float)x) / 60.0f, 256),
					perlinz = fmod((position.z * (float)Chunk::chunkSizeXZ + (float)z) / 60.0f, 256);
				if (perlinx < 0)
					perlinx += 256;
				if (perlinz < 0)
					perlinz += 256;
				int height = (db::perlin<float>(perlinx, perlinz) + 1.0f) * 30 + 1;
				for (unsigned int y = 0; y < Chunk::chunkSizeY; y++)
				{
					Block block = Block(Block::BlockType::AIR);

					if (position.y < 0)
						block.type = Block::BlockType::STONE;
					else if (position.y == 0)
					{
						if (y == height)
							block.type = Block::BlockType::GRASS;
						else if (y >= height - 3 && y < height)
							block.type = Block::BlockType::MUD;
						else if (y < height - 3)
							block.type = Block::BlockType::STONE;
					}

					if (block.type != Block::BlockType::AIR)
						this->SetBlock(glm::uvec3(x, y, z), block);
				}
			}
		generatingStatus += step;
		if (generatingStatus >= Chunk::chunkSizeXZ)
		{
			done = true; 
			UpdateMesh();
		}
	}
};*/

/*class FontManager Unnessesery
{
private:
	bool setup()
	{
		
	}
public:
	FontManager()
	{
		setup();
	}
	static FontManager& GetInstance()
	{
		static FontManager instance;
		return instance;
	}
};
*/






























class Chunk
{
public:
	static const int chunkSizeXZ = 16,
		chunkSizeY = 64;
protected:
	Block block[chunkSizeXZ][chunkSizeY][chunkSizeXZ];
	unsigned int vbo = 0, 
		vao = 0, 
		ebo = 0,
		indicesCount = 0;
	glm::ivec3 highlightedBlock = glm::ivec3(0);
	bool highlighted = false;
	const float highlightedLight = 0.3f;

	std::vector<Vertex3D> vertices;
	std::vector<unsigned int> indices;

	struct BlockData
	{
		int xp, xm,
			yp, ym,
			zp, zm; //X, Y, Z + (P)LUS or (M)INUS

		BlockData(int xp, int xm, int yp, int ym, int zp, int zm) :
			xp(xp),
			xm(xm),
			yp(yp),
			ym(ym),
			zp(zp),
			zm(zm)
		{

		}

		BlockData() :
			BlockData(-1, -1, -1, -1, -1, -1)
		{

		}
	};
	/*std::unordered_map<int, BlockData> blockData;

	struct BlockChange
	{
		enum class ChangeType
		{
			HIGHLIGHT,
			DESTROY,
			PLACE
		};
		glm::ivec3 pos;
		ChangeType type;
		
		struct HighlightParams
		{
			bool on;
		}; 
		
		struct PlaceParams
		{
			Block::BlockType type;
		};

		union
		{
			HighlightParams highlight;
			PlaceParams place;
		} params;
	};

	std::queue<BlockChange> changes;*/

	glm::ivec3 lastBlock = glm::ivec3(0);
	const unsigned int stepSize = 40960;

	bool toUpdate = false,
		loaded = false;

	void pushFace(std::vector<Vertex3D> &vertices, 
		std::vector<unsigned int> &indices, 
		const unsigned int vertexIndices[4], 
		glm::ivec3 position, 
		Block::BlockType type,
		bool highlighted)
	{
		const float verticesc[5 * 14] = {
		   0, 0, 0, 0, 0.333333333f,
		   0, 1, 0, 0.25f, 0.333333f,
		   1, 1, 0, 0.25f, 0.66666f,
		   1, 0, 0, 0, 0.666666f,
		   0, 0, 1, 0.75f, 0.33333f,
		   0, 1, 1, 0.5f, 0.33333f,
		   1, 1, 1, 0.5f, 0.66666f,
		   1, 0, 1, 0.75, 0.66666f,
		   0, 0, 0, 1, 0.333333f,
		   1, 0, 0, 1, 0.666666f,
		   0, 0, 0, 0.25f, 0,
		   0, 0, 1, 0.5f, 0,
		   1, 0, 0, 0.25f, 1,
		   1, 0, 1, 0.5f, 1 };

		Vertex3D vertex;

		AtlasTexture::SubTexture& st = Block::GetSubTexture(type);

		for (int i = 0; i < 4; i++)
		{
			vertex.x = (verticesc[5 * vertexIndices[i] + 0] + position.x);
			vertex.y = (verticesc[5 * vertexIndices[i] + 1] + position.y);
			vertex.z = (verticesc[5 * vertexIndices[i] + 2] + position.z);

			vertex.u = st.uv.x;
			vertex.v = st.uv.y;

			vertex.u += (verticesc[5 * vertexIndices[i] + 3] * st.size.x);
			vertex.v += (verticesc[5 * vertexIndices[i] + 4] * st.size.y);

			vertex.light = highlighted ? highlightedLight : 0.0f;
			vertices.push_back(vertex);
		}

		const unsigned int size = vertices.size();
		indices.push_back(size - 4);
		indices.push_back(size - 2);
		indices.push_back(size - 3);

		indices.push_back(size - 2);
		indices.push_back(size - 1);
		indices.push_back(size - 3);
	}

	void pushBlock(std::vector<Vertex3D> &vertices, std::vector<unsigned int> &indices, glm::ivec3 position, Block &current)
	{
		if (!IsBetween(position.x, 0, chunkSizeXZ) ||
			!IsBetween(position.y, 0, chunkSizeY) ||
			!IsBetween(position.z, 0, chunkSizeXZ))
			return;

		static const Block::BlockType airID = Block::GetIdByName("Air");
		const bool highlighted = this->highlighted && (position == highlightedBlock);

		BlockData data; //TODO: REMOVE

		if (current.type == airID)
			return;

		if (position.x == chunkSizeXZ - 1 || (position.x < chunkSizeXZ - 1 && block[position.x + 1][position.y][position.z].type == airID))
		{
			const unsigned int vertexIndices[4] = { 2, 6, 12, 13 };

			data.xp = indices.size(); //Size is an index of a next element;
			pushFace(vertices, indices, vertexIndices, position, current.type, highlighted);
		}
		if (position.x == 0 || (position.x > 0 && block[position.x - 1][position.y][position.z].type == airID))
		{
			const unsigned int vertexIndices[4] = { 5, 1, 11, 10 };

			data.xm = indices.size();
			pushFace(vertices, indices, vertexIndices, position, current.type, highlighted);
		}
		if (position.y == chunkSizeY - 1 || (position.y < chunkSizeY - 1 && block[position.x][position.y + 1][position.z].type == airID))
		{
			const unsigned int vertexIndices[4] = { 5, 6, 1, 2 };

			data.yp = indices.size();
			pushFace(vertices, indices, vertexIndices, position, current.type, highlighted);
		}
		if (position.y == 0 || (position.y > 0 && block[position.x][position.y - 1][position.z].type == airID))
		{
			const unsigned int vertexIndices[4] = { 4, 8, 7, 9 };

			data.ym = indices.size();
			pushFace(vertices, indices, vertexIndices, position, current.type, highlighted);
		}
		if (position.z == chunkSizeXZ - 1 || (position.z < chunkSizeXZ - 1 && block[position.x][position.y][position.z + 1].type == airID))
		{
			const unsigned int vertexIndices[4] = { 5, 4, 6, 7 };

			data.zp = indices.size();
			pushFace(vertices, indices, vertexIndices, position, current.type, highlighted);
		}
		if (position.z == 0 || (position.z > 0 && block[position.x][position.y][position.z - 1].type == airID))
		{
			const unsigned int vertexIndices[4] = { 0, 1, 3, 2 };

			data.zm = indices.size();
			pushFace(vertices, indices, vertexIndices, position, current.type, highlighted);
		}

		//blockData[convertPosToMapIndex(position)] = data;
	}

	void setupBuffers()
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo); 
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)(5 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glGenBuffers(1, &ebo);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void updateBuffers()
	{
		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex3D), vertices.data(), GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		indicesCount = indices.size();
	}

	void clearVectors()
	{
		vertices.clear();
		indices.clear();

		std::vector<Vertex3D>().swap(vertices);
		std::vector<unsigned int>().swap(indices);
	}

	int convertPosToMapIndex(glm::ivec3 pos)
	{
		if (pos.x < 0 || pos.x >= chunkSizeXZ ||
			pos.y < 0 || pos.y >= chunkSizeY ||
			pos.z < 0 || pos.z >= chunkSizeXZ)
			return -1;
		return pos.x + pos.y * chunkSizeXZ + pos.z * chunkSizeXZ * chunkSizeY;
	}
public:
	Chunk()
	{

	}

	void CreateMesh(bool clearChanges = true)
	{
		toUpdate = false;

		/*if(clearChanges)
			while (!changes.empty()) changes.pop(); //Clear any changes, there are not needed more*/
		if (!vao)
			setupBuffers();

		clearVectors();

		for (int x = 0; x < chunkSizeXZ; x++)
		{
			for (int y = 0; y < chunkSizeY; y++)
			{
				for (int z = 0; z < chunkSizeXZ; z++)
				{
					const glm::ivec3 pos = { x, y, z };
					pushBlock(vertices, indices, pos, block[x][y][z]);
				}
			}
		}

		loaded = true;
		updateBuffers();
		clearVectors();
	}

	/*void UpdateMesh()
	{
		while (!changes.empty())
		{
			BlockChange &change = changes.front();
			BlockData &block = blockData[convertPosToMapIndex(change.pos)];
			switch(change.type)
			{
			case BlockChange::ChangeType::HIGHLIGHT:
			{
				/*const int index[6] = { block.xp,
					block.xm,
					block.yp,
					block.ym,
					block.zp,
					block.zm };

				for (int i = 0; i < 6; i++) //For every face
					if (index[i] != -1) //Face is visible
					{
						for (int j = 0; j < 6; j++) // For every vertex index in face
							vertices[indices[index[i] + j]].light = change.params.highlight.on ? highlightedLight : 0.0f;
					}*/
				//CreateMesh(false);
			/*	break;
			}
			case BlockChange::ChangeType::PLACE:
			{
				Block b = Block(change.params.place.type);
				pushBlock(change.pos, b);
				break;
			}
			case BlockChange::ChangeType::DESTROY:
			{
				/*const int index[6] = { block.xp,
					block.xm,
					block.yp,
					block.ym,
					block.zp,
					block.zm };

				std::vector<unsigned int> verticesToRemove,
					indicesToRemove;
				for (int i = 0; i < 6; i++) //For every face
					if (index[i] != -1) //Face is visible
					{
						for (int j = 0; j < 6; j++) // For every vertex index in face
						{
							const int currentVertexIndex = indices[index[i] + j];
							if (std::find(verticesToRemove.begin(), verticesToRemove.end(), currentVertexIndex) == verticesToRemove.end()) //To not to insert same number twice
							{
								verticesToRemove.push_back(currentVertexIndex);
								//vertices.erase(vertices.begin() + indices[index[i] + j]);
							}
						}
					}

				for (int i = 0; i < 6; i++) //For every face
					if (index[i] != -1) //Face is visible
					{
						for (int j = 0; j < 4; j++) // For every index in face
						{
							indicesToRemove.push_back(index[i] + j); //Add to removing "queue"
						}
					}

				std::sort(indicesToRemove.begin(), indicesToRemove.end(), std::greater<unsigned int>());
					
				for (auto it = indicesToRemove.begin(); it != indicesToRemove.end(); it++)
				{
					indices.erase(indices.begin() + *it);
					for (auto data = blockData.begin(); data != blockData.end(); data++)
					{
						if ((*data).second.xp > (int)*it) (*data).second.xp--;
						if ((*data).second.xm > (int)*it) (*data).second.xm--;
						if ((*data).second.yp > (int)*it) (*data).second.yp--;
						if ((*data).second.ym > (int)*it) (*data).second.ym--;
						if ((*data).second.zp > (int)*it) (*data).second.zp--;
						if ((*data).second.zm > (int)*it) (*data).second.zm--;
					}
				}

				std::sort(verticesToRemove.begin(), verticesToRemove.end(), std::greater<unsigned int>());

				int debug = 0;
				for (auto it = verticesToRemove.begin(); it != verticesToRemove.end(); it++)
				{
					std::cout << debug << "  " << vertices[*it].light << std::endl;
					debug++;
					vertices.erase(vertices.begin() + *it);
					for (auto ind = indices.begin(); ind != indices.end(); ind++)
						if (*ind > *it)
							(*ind)--;
				} 
				
				blockData.erase(convertPosToMapIndex(change.pos));*/
			/*	CreateMesh(false);
				break;
			}
			}
			changes.pop();
		}
		
		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex3D), vertices.data(), GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		indicesCount = indices.size();
	}*/

	void Update()
	{
		if (toUpdate)
			CreateMesh();
	}

	void Render()
	{
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

		glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
	}

	std::pair<Block, bool> GetBlock(glm::uvec3 pos) //LOCAL POSITION
	{
		if (!BETWEEN(pos.x, 0, chunkSizeXZ) ||
			!BETWEEN(pos.y, 0, chunkSizeY) ||
			!BETWEEN(pos.z, 0, chunkSizeXZ))
			return { Block(), false };
	
		return { block[pos.x][pos.y][pos.z], true };
	}
	
	bool SetBlock(glm::uvec3 pos, Block &newBlock, bool update = true) //LOCAL POSITION
	{
		if (!BETWEEN(pos.x, 0, chunkSizeXZ) ||
			!BETWEEN(pos.y, 0, chunkSizeY) ||
			!BETWEEN(pos.z, 0, chunkSizeXZ))
			return false;

		block[pos.x][pos.y][pos.z] = newBlock;

		/*if (registerChange)
		{
			BlockChange bc;
			bc.pos = pos;
			if (newBlock.type != Block::BlockType::AIR)
			{
				bc.type = BlockChange::ChangeType::PLACE;
				bc.params.place.type = newBlock.type;
			}
			else
				bc.type = BlockChange::ChangeType::DESTROY;
			changes.push(bc);
		}*/
		if(toUpdate)
			toUpdate = true;
		return true;
	}

	void SetHighlightedBlock(glm::ivec3 position)
	{
		/*BlockChange bc;
		bc.pos = highlightedBlock;
		bc.type = BlockChange::ChangeType::HIGHLIGHT;
		bc.params.highlight.on = false;
		changes.push(bc); //De-highlight last block*/

		highlighted = true;
		highlightedBlock = position; 

		/*bc.pos = position;
		bc.params.highlight.on = true;
		changes.push(bc); //Highlight new block*/

		toUpdate = true;
	}

	void DisableHighlightedBlock()
	{
		highlighted = false;

		/*BlockChange bc;
		bc.pos = highlightedBlock;
		bc.type = BlockChange::ChangeType::HIGHLIGHT;
		bc.params.highlight.on = false;
		changes.push(bc); //De-highlight last block*/
		
		toUpdate = true;
	}
	
	bool CreateMeshPartially()
	{
		//while (!changes.empty()) changes.pop(); //Clear any changes, there are not needed more
		if (!vao)
			setupBuffers();

		if (lastBlock == glm::ivec3(0))
			clearVectors();

		int& x = lastBlock.x,
			& y = lastBlock.y,
			& z = lastBlock.z,
			counter = 0;

		for (; x < chunkSizeXZ; x++)
		{
			for (; y < chunkSizeY; y++)
			{
				for (; z < chunkSizeXZ; z++)
				{
					const glm::ivec3 pos = { x, y, z };
					pushBlock(vertices, indices, pos, block[x][y][z]);

					counter++;
					if (counter > stepSize)
						break;
				}
				if (counter > stepSize)
					break;
				z = 0;
			}
			if (counter > stepSize)
				break;
			y = 0;
		}

		if (x == chunkSizeXZ) //Generating done
		{
			updateBuffers();
			clearVectors();
			loaded = true;
			return true;
		}
		return false;
	}

	bool IsLoaded()
	{
		return loaded;
	}
};




bool Chunk::CreateMeshPartially()
{
	if (lastBlock == glm::ivec3(0))
	{
		vboSolid.ClearVertices();
		vboTransparent.ClearVertices();
	}

	int& x = lastBlock.x,
		& y = lastBlock.y,
		& z = lastBlock.z,
		counter = 0;

	Chunk* neighborhood[6] = { world.GetChunkByCoord(position + glm::ivec3(1, 0, 0)),
	world.GetChunkByCoord(position + glm::ivec3(-1, 0, 0)),
	world.GetChunkByCoord(position + glm::ivec3(0, 1, 0)),
	world.GetChunkByCoord(position + glm::ivec3(0, -1, 0)),
	world.GetChunkByCoord(position + glm::ivec3(0, 0, 1)),
	world.GetChunkByCoord(position + glm::ivec3(0, 0, -1)) };
	
	for (; x < chunkSizeXZ; x++)
	{
		for (; y < chunkSizeY; y++)
		{
			for (; z < chunkSizeXZ; z++)
			{
				const glm::ivec3 pos = { x, y, z };
				pushBlock(pos, block[x][y][z], neighborhood);

				counter++;
				if (counter > stepSize)
					break;
			}
			if (counter > stepSize)
				break;
			z = 0;
		}
		if (counter > stepSize)
			break;
		y = 0;
	}

	if (x == chunkSizeXZ) //Generating done
	{
		vboSolid.UpdateBuffers();
		vboTransparent.UpdateBuffers();
		vboSolid.ClearVertices();
		vboTransparent.ClearVertices();
		loaded = true;
		return true;
	}
	return false;
}
