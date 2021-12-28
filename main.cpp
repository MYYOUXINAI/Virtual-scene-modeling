#include "Angel.h"
#include "TriMesh.h"
#include "Camera.h"
#include "MeshPainter.h"

#include <vector>
#include <string>

int WIDTH = 600;
int HEIGHT = 600;

int mainWindow;

//用于记录帧数
int state = 0;

Camera* camera = new Camera();
Light* light = new Light();
MeshPainter *painter = new MeshPainter();
MeshPainter* painter2 = new MeshPainter();
MeshPainter* sky_painter = new MeshPainter();

//单独存放机器人的各个部分
MeshPainter* painter_hat = new MeshPainter();
MeshPainter* painter_head = new MeshPainter();
MeshPainter* painter_body = new MeshPainter();
MeshPainter* painter_left_upper_arm = new MeshPainter();
MeshPainter* painter_right_upper_arm = new MeshPainter();
MeshPainter* painter_left_lower_arm = new MeshPainter();
MeshPainter* painter_right_lower_arm = new MeshPainter();
MeshPainter* painter_left_upper_leg = new MeshPainter();
MeshPainter* painter_right_upper_leg = new MeshPainter();
MeshPainter* painter_left_lower_leg = new MeshPainter();
MeshPainter* painter_right_lower_leg = new MeshPainter();


glm::vec3 cameraPosition(0.0, 8, 0.0);		//eye
glm::vec3 cameraDirection(0.0, -1.0, 0.0);		//at
glm::vec3 cameraUp(0.0, 1.0, 0.0);		//up


// 这个用来回收和删除我们创建的物体对象
std::vector<TriMesh *> meshList;
//to draw floor
std::vector<TriMesh*> meshList2;
//to draw robot
std::vector<TriMesh*>meshList3;

//to draw skybox
std::vector<TriMesh*>meshList4;

//用于保存robot的变换矩阵
class MatrixStack {
	int		_index;
	int		_size;
	glm::mat4* _matrices;

public:
	MatrixStack(int numMatrices = 100) :_index(0), _size(numMatrices)
	{
		_matrices = new glm::mat4[numMatrices];
	}

	~MatrixStack()
	{
		delete[]_matrices;
	}

	void push(const glm::mat4& m) {
		assert(_index + 1 < _size);
		_matrices[_index++] = m;
	}

	glm::mat4& pop() {
		assert(_index - 1 >= 0);
		_index--;
		return _matrices[_index];
	}
};

//用于按键控制人物移动
glm::mat4 keyboard_matrix=glm::mat4(1.0);

//控制物体位移变换的参数
float _x = 0.0f;
float _y = 0.0f;
float _z = 0.0f;
//控制物体角度变换的参数
float _X = 0.0f;
float _Y = 0.0f;
float _Z = 0.0f;

//将robot的模型全局化
TriMesh* hat = new TriMesh();
TriMesh* head = new TriMesh();
TriMesh* body = new TriMesh();
TriMesh* left_upper_arm = new TriMesh();
TriMesh* right_upper_arm = new TriMesh();
TriMesh* left_lower_arm = new TriMesh();
TriMesh* right_lower_arm = new TriMesh();
TriMesh* left_upper_leg = new TriMesh();
TriMesh* right_upper_leg = new TriMesh();
TriMesh* left_lower_leg = new TriMesh();
TriMesh* right_lower_leg = new TriMesh();

//全局化关节角
float theta[11] = {
	0.0,//hat
	0.0,//head
	0.0,//body
	0.0,//left_upper_arm
	0.0,//right_upper_arm
	0.0,//left_lower_arm
	0.0,//right_lower_arm
	0.0,//left_upper_leg
	0.0,//right_upper_leg
	0.0,//left_lower_leg
	0.0//right_lower_leg
};

//单一绘制hat
void display_hat(glm::mat4 modelMatrix)
{
	painter_hat->drawRobot(light, camera, modelMatrix);
}

//单一绘制head
void display_head(glm::mat4 modelMatrix)
{
	painter_head->drawRobot(light, camera, modelMatrix);
}

//单一绘制body
void display_body(glm::mat4 modelMatrix)
{
	painter_body->drawRobot(light, camera, modelMatrix);
}

//单一绘制left_upper_arm
void display_left_upper_arm(glm::mat4 modelMatrix)
{
	/*glm::mat4 instance = glm::mat4(1.0);
	instance = glm::translate(instance,glm::vec3(0.0, 0.0, 0.0));*/
	painter_left_upper_arm->drawRobot(light, camera, modelMatrix);
}

//单一绘制right_upper_arm
void display_right_upper_arm(glm::mat4 modelMatrix)
{
	painter_right_upper_arm->drawRobot(light, camera, modelMatrix);
}

//单一绘制left_lower_arm
void display_left_lower_arm(glm::mat4 modelMatrix)
{
	painter_left_lower_arm->drawRobot(light, camera, modelMatrix);
}

//单一绘制right_lower_arm
void display_right_lower_arm(glm::mat4 modelMatrix)
{
	painter_right_lower_arm->drawRobot(light, camera, modelMatrix);
}

//单一绘制left_upper_leg
void display_left_upper_leg(glm::mat4 modelMatrix)
{
	painter_left_upper_leg->drawRobot(light, camera, modelMatrix);
}

//单一绘制right_upper_leg
void display_right_upper_leg(glm::mat4 modelMatrix)
{
	painter_right_upper_leg->drawRobot(light, camera, modelMatrix);
}


//单一绘制left_lower_leg
void display_left_lower_leg(glm::mat4 modelMatrix)
{
	painter_left_lower_leg->drawRobot(light, camera, modelMatrix);
}


//单一绘制right_lower_leg
void display_right_lower_leg(glm::mat4 modelMatrix)
{
	painter_right_lower_leg->drawRobot(light, camera, modelMatrix);
}



//用于进行机器人的旋转、位移变换
void display_robot()
{
	//物体的变换矩阵
	glm::mat4 modelMatrix = glm::mat4(1.0);
	//glm::mat4 modelMatrix = glm::translate(test,glm::vec3(0.6,0.6,0.6));
	//modelMatrix = keyboard_matrix * modelMatrix;

	//用于保存物体变换矩阵的栈
	MatrixStack mstack;

	//body
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, 0.0, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(theta[2]), glm::vec3(0.0, 1.0, 0.0));
	display_body(modelMatrix);

	//保存躯干变换矩阵
	mstack.push(modelMatrix);
	
	//head
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, 0.0, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(theta[1]), glm::vec3(0.0, 1.0, 0.0));
	display_head(modelMatrix);

	//保存头部变换矩阵
	mstack.push(modelMatrix);

	//hat
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, 0.0, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(theta[0]), glm::vec3(0.0, 1.0, 0.0));
	display_hat(modelMatrix);

	//头部变换矩阵出栈
	mstack.pop();
	//躯干变换矩阵出栈，并且还原
	modelMatrix = mstack.pop();
	//保存躯干变换矩阵
	mstack.push(modelMatrix);


	//left_upper_arm
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.65, 0.0, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(theta[3]), glm::vec3(1.0, 0.0, 0.0));
	display_left_upper_arm(modelMatrix);

	//左上臂变换矩阵入栈
	mstack.push(modelMatrix);

	//left_lower_arm
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.50, -0.85, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(theta[5]), glm::vec3(1.0, 0.0, 0.0));
	display_left_lower_arm(modelMatrix);

	//还原躯干变换矩阵
	mstack.pop();
	modelMatrix = mstack.pop();
	//保存躯干变换矩阵
	mstack.push(modelMatrix);

	//right_upper_arm
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.65, 0.0, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(theta[4]), glm::vec3(1.0, 0.0, 0.0));
	display_right_upper_arm(modelMatrix);

	//保存右上臂变换矩阵
	mstack.push(modelMatrix);

	//right_lower_arm
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.50, -0.85, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(theta[6]), glm::vec3(1.0, 0.0, 0.0));
	display_right_lower_arm(modelMatrix);

	//还原躯干变换矩阵
	mstack.pop();
	modelMatrix = mstack.pop();

	//保存躯干变换矩阵
	mstack.push(modelMatrix);

	//left_upper_leg
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.4, 0.0, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(theta[7]), glm::vec3(1.0, 0.0, 0.0));
	display_left_upper_leg(modelMatrix);

	//保存左大腿变换矩阵
	mstack.push(modelMatrix);

	//left_lower_leg
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.025, -0.75, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(theta[9]), glm::vec3(1.0, 0.0, 0.0));
	display_left_lower_leg(modelMatrix);

	//还原躯干变换矩阵
	mstack.pop();
	modelMatrix = mstack.pop();

	//保存躯干变换矩阵
	mstack.push(modelMatrix);

	//right_upper_leg
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.4,0.0,0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(theta[8]),glm::vec3(1.0,0.0,0.0));
	display_right_upper_leg(modelMatrix);

	//保存右大腿变换矩阵
	mstack.push(modelMatrix);

	//right_lower_leg
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.025, -0.75, 0.0));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(theta[10]), glm::vec3(1.0, 0.0, 0.0));
	display_right_lower_leg(modelMatrix);

}


//用于绘制地面
void display_floor()
{
	
	std::string vshader, fshader;
	// 读取着色器并使用
	vshader = "shaders/vshader.glsl";
	fshader = "shaders/fshader.glsl";
	TriMesh* floor = new TriMesh();
	// floor
	floor->setNormalize(true);
	floor->readObj("./assets/floor.obj");

	// 设置物体的旋转位移
	floor->setTranslation(glm::vec3(0, -0.04, -10.0));
	floor->setRotation(glm::vec3(0, 0, 0));
	floor->setScale(glm::vec3(200.0, 3.0, 200.0));

	floor->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
	floor->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
	floor->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
	floor->setShininess(1.0); //高光系数

   // 加到painter中
	painter2->addMesh(floor, "mesh_a", "./assets/floor.png", vshader, fshader);
	meshList2.push_back(floor);
}



void init()
{
	std::string vshader, fshader;
	// 读取着色器并使用
	vshader = "shaders/vshader.glsl";
	fshader = "shaders/fshader.glsl";

	// 设置光源位置
	light->setTranslation(glm::vec3(0.0, 10.0, 10.0));
	light->setAmbient(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 环境光
	light->setDiffuse(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 漫反射
	light->setSpecular(glm::vec4(1.0, 1.0, 1.0, 1.0)); // 镜面反射
	light->setAttenuation(1.0, 0.045, 0.0075); // 衰减系数


	//to draw skybox
	if (true)
	{

		//right
		TriMesh* sky_right = new TriMesh();
		sky_right->setNormalize(true);
		// 创建圆柱体
		sky_right->generateSquareRight(glm::vec3(0.0, 0.0, 0.0));
		//exit(-1);
		// 设置物体的旋转位移
		sky_right->setTranslation(glm::vec3(0.0, 0.0, 0.0));
		sky_right->setRotation(glm::vec3(0.0, 0.0, 0.0));
		sky_right->setScale(glm::vec3(60.0, 60.0, 60.0));

		sky_right->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
		sky_right->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
		sky_right->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
		sky_right->setShininess(1.0); //高光系数
		// 加到painter中
		sky_painter->addMesh(sky_right, "mesh_a", "./assets/right.jpg", vshader, fshader); 	// 指定纹理与着色器
		// 我们创建的这个加入一个容器内，为了程序结束时将这些数据释放
		meshList4.push_back(sky_right);


		//left
		TriMesh* sky_left = new TriMesh();
		sky_left->setNormalize(true);
		// 创建圆柱体
		sky_left->generateSquareLeft(glm::vec3(0.0, 0.0, 0.0));
		//exit(-1);
		// 设置物体的旋转位移
		sky_left->setTranslation(glm::vec3(0.0, 0.0, 0.0));
		sky_left->setRotation(glm::vec3(0.0, 0.0, 0.0));
		sky_left->setScale(glm::vec3(60.0, 60.0, 60.0));

		sky_left->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
		sky_left->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
		sky_left->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
		sky_left->setShininess(1.0); //高光系数
		// 加到painter中
		sky_painter->addMesh(sky_left, "mesh_a", "./assets/left.jpg", vshader, fshader); 	// 指定纹理与着色器
		// 我们创建的这个加入一个容器内，为了程序结束时将这些数据释放
		meshList4.push_back(sky_left);


		//top
		TriMesh* sky_top = new TriMesh();
		sky_top->setNormalize(true);
		// 创建圆柱体
		sky_top->generateSquareTop(glm::vec3(0.0, 0.0, 0.0));
		//exit(-1);
		// 设置物体的旋转位移
		sky_top->setTranslation(glm::vec3(0.0, 0.0, 0.0));
		sky_top->setRotation(glm::vec3(0.0, 0.0, 0.0));
		sky_top->setScale(glm::vec3(60.0, 60.0, 60.0));

		sky_top->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
		sky_top->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
		sky_top->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
		sky_top->setShininess(1.0); //高光系数
		// 加到painter中
		sky_painter->addMesh(sky_top, "mesh_a", "./assets/top.jpg", vshader, fshader); 	// 指定纹理与着色器
		// 我们创建的这个加入一个容器内，为了程序结束时将这些数据释放
		meshList4.push_back(sky_top);


		//buttom
		TriMesh* sky_buttom = new TriMesh();
		sky_buttom->setNormalize(true);
		// 创建圆柱体
		sky_buttom->generateSquareButtom(glm::vec3(0.0, 0.0, 0.0));
		//exit(-1);
		// 设置物体的旋转位移
		sky_buttom->setTranslation(glm::vec3(0.0, 0.0, 0.0));
		sky_buttom->setRotation(glm::vec3(0.0, 0.0, 0.0));
		sky_buttom->setScale(glm::vec3(60.0, 60.0, 60.0));

		sky_buttom->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
		sky_buttom->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
		sky_buttom->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
		sky_buttom->setShininess(1.0); //高光系数
		// 加到painter中
		sky_painter->addMesh(sky_buttom, "mesh_a", "./assets/Buttom.jpg", vshader, fshader); 	// 指定纹理与着色器
		// 我们创建的这个加入一个容器内，为了程序结束时将这些数据释放
		meshList4.push_back(sky_buttom);


		//back
		TriMesh* sky_back = new TriMesh();
		sky_back->setNormalize(true);
		// 创建圆柱体
		sky_back->generateSquareBack(glm::vec3(0.0, 0.0, 0.0));
		//exit(-1);
		// 设置物体的旋转位移
		sky_back->setTranslation(glm::vec3(0.0, 0.0, 0.0));
		sky_back->setRotation(glm::vec3(0.0, 0.0, 0.0));
		sky_back->setScale(glm::vec3(60.0, 60.0, 60.0));

		sky_back->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
		sky_back->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
		sky_back->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
		sky_back->setShininess(1.0); //高光系数
		// 加到painter中
		sky_painter->addMesh(sky_back, "mesh_a", "./assets/front.jpg", vshader, fshader); 	// 指定纹理与着色器
		// 我们创建的这个加入一个容器内，为了程序结束时将这些数据释放
		meshList4.push_back(sky_back);


		//front
		TriMesh* sky_front = new TriMesh();
		sky_front->setNormalize(true);
		// 创建圆柱体
		sky_front->generateSquareFront(glm::vec3(0.0, 0.0, 0.0));
		//exit(-1);
		// 设置物体的旋转位移
		sky_front->setTranslation(glm::vec3(0.0, 0.0, 0.0));
		sky_front->setRotation(glm::vec3(0.0, 0.0, 0.0));
		sky_front->setScale(glm::vec3(60.0, 60.0, 60.0));

		sky_front->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
		sky_front->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
		sky_front->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
		sky_front->setShininess(1.0); //高光系数
		// 加到painter中
		sky_painter->addMesh(sky_front, "mesh_a", "./assets/back.jpg", vshader, fshader); 	// 指定纹理与着色器
		// 我们创建的这个加入一个容器内，为了程序结束时将这些数据释放
		meshList4.push_back(sky_front);
	}
	
	// hat
	hat->setNormalize(true);
	hat->readObj("./assets/hat.obj");

	// 设置物体的旋转位移
	hat->setTranslation(glm::vec3(0, 1.75, 0.0));
	hat->setRotation(glm::vec3(0, 0, 0));
	hat->setScale(glm::vec3(1.0, 1.0, 1.0));

	hat->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
	hat->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
	hat->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
	hat->setShininess(1.0); //高光系数

   // 加到painter中
	painter_hat->addMesh(hat, "mesh_a", "./assets/hat.png", vshader, fshader);
	meshList3.push_back(hat);



	
	//head
	head->setNormalize(true);
	head->readObj("./assets/head.obj");

	// 设置物体的旋转位移
	head->setTranslation(glm::vec3(0, 1.35, 0.0));
	head->setRotation(glm::vec3(0, 0, 0));
	head->setScale(glm::vec3(1.0, 1.0, 1.0));

	head->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
	head->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
	head->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
	head->setShininess(1.0); //高光系数

   // 加到painter中
	painter_head->addMesh(head, "mesh_a", "./assets/head.png", vshader, fshader);
	meshList3.push_back(head);



	
	//body
	body->setNormalize(true);
	body->readObj("./assets/body.obj");

	// 设置物体的旋转位移
	body->setTranslation(glm::vec3(0, 0.875, 0.0));
	body->setRotation(glm::vec3(0, 0, 0));
	body->setScale(glm::vec3(1.0, 1.0, 1.0));

	body->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
	body->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
	body->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
	body->setShininess(1.0); //高光系数

   // 加到painter中
	painter_body->addMesh(body, "mesh_a", "./assets/body.png", vshader, fshader);
	meshList3.push_back(body);



	
	// left_upper_arm
	left_upper_arm->setNormalize(true);
	left_upper_arm->readObj("./assets/left_upper_arm.obj");

	// 设置物体的旋转位移
	//left_upper_arm->setTranslation(glm::vec3(-0.35, 0.875, 0.0));
	left_upper_arm->setTranslation(glm::vec3(0.0, 0.875, 0.0));
	left_upper_arm->setRotation(glm::vec3(0, 0, 0));
	left_upper_arm->setScale(glm::vec3(0.5, 0.5, 0.5));

	left_upper_arm->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
	left_upper_arm->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
	left_upper_arm->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
	left_upper_arm->setShininess(1.0); //高光系数

   // 加到painter中
	painter_left_upper_arm->addMesh(left_upper_arm, "mesh_a", "./assets/left_upper_arm.png", vshader, fshader);
	meshList3.push_back(left_upper_arm);




	
	// right_upper_arm
	right_upper_arm->setNormalize(true);
	right_upper_arm->readObj("./assets/right_upper_arm.obj");

	// 设置物体的旋转位移
	//right_upper_arm->setTranslation(glm::vec3(0.35, 0.875, 0.0));
	right_upper_arm->setTranslation(glm::vec3(0.0, 0.875, 0.0));
	right_upper_arm->setRotation(glm::vec3(0, 0, 0));
	right_upper_arm->setScale(glm::vec3(0.5, 0.5, 0.5));

	right_upper_arm->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
	right_upper_arm->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
	right_upper_arm->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
	right_upper_arm->setShininess(1.0); //高光系数

   // 加到painter中
	painter_right_upper_arm->addMesh(right_upper_arm, "mesh_a", "./assets/right_upper_arm.png", vshader, fshader);
	meshList3.push_back(right_upper_arm);



	
	// left_lower_arm
	left_lower_arm->setNormalize(true);
	left_lower_arm->readObj("./assets/lower_arm.obj");

	// 设置物体的旋转位移
	//left_lower_arm->setTranslation(glm::vec3(-0.435,0.625, 0.0));
	left_lower_arm->setTranslation(glm::vec3(0.0, 0.875, 0.0));
	left_lower_arm->setRotation(glm::vec3(0, 0, 0));
	left_lower_arm->setScale(glm::vec3(0.35, 0.35, 0.35));

	left_lower_arm->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
	left_lower_arm->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
	left_lower_arm->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
	left_lower_arm->setShininess(1.0); //高光系数

   // 加到painter中
	painter_left_lower_arm->addMesh(left_lower_arm, "mesh_a", "./assets/lower_arm.png", vshader, fshader);
	meshList3.push_back(left_lower_arm);


	
	// right_lower_arm
	right_lower_arm->setNormalize(true);
	right_lower_arm->readObj("./assets/lower_arm.obj");

	// 设置物体的旋转位移
	//right_lower_arm->setTranslation(glm::vec3(0.435, 0.625, 0.0));
	right_lower_arm->setTranslation(glm::vec3(0.0, 0.875, 0.0));
	right_lower_arm->setRotation(glm::vec3(0, 0, 0));
	right_lower_arm->setScale(glm::vec3(0.35, 0.35, 0.35));

	right_lower_arm->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
	right_lower_arm->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
	right_lower_arm->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
	right_lower_arm->setShininess(1.0); //高光系数

   // 加到painter中
	painter_right_lower_arm->addMesh(right_lower_arm, "mesh_a", "./assets/lower_arm.png", vshader, fshader);
	meshList3.push_back(right_lower_arm);




	
	// left_upper_leg
	left_upper_leg->setNormalize(true);
	left_upper_leg->readObj("./assets/upper_leg.obj");

	// 设置物体的旋转位移
	//left_upper_leg->setTranslation(glm::vec3(-0.15, 0.4, 0.0));
	left_upper_leg->setTranslation(glm::vec3(0.0, 0.4, 0.0));
	left_upper_leg->setRotation(glm::vec3(0, 0, 0));
	left_upper_leg->setScale(glm::vec3(0.40, 0.40, 0.40));

	left_upper_leg->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
	left_upper_leg->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
	left_upper_leg->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
	left_upper_leg->setShininess(1.0); //高光系数

   // 加到painter中
	painter_left_upper_leg->addMesh(left_upper_leg, "mesh_a", "./assets/upper_leg.png", vshader, fshader);
	meshList3.push_back(left_upper_leg);





	
	// right_upper_leg
	right_upper_leg->setNormalize(true);
	right_upper_leg->readObj("./assets/upper_leg.obj");

	// 设置物体的旋转位移
	//right_upper_leg->setTranslation(glm::vec3(0.15, 0.4, 0.0));
	right_upper_leg->setTranslation(glm::vec3(0.0, 0.4, 0.0));
	right_upper_leg->setRotation(glm::vec3(0, 0, 0));
	right_upper_leg->setScale(glm::vec3(0.40, 0.40, 0.40));

	right_upper_leg->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
	right_upper_leg->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
	right_upper_leg->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
	right_upper_leg->setShininess(1.0); //高光系数

   // 加到painter中
	painter_right_upper_leg->addMesh(right_upper_leg, "mesh_a", "./assets/upper_leg.png", vshader, fshader);
	meshList3.push_back(right_upper_leg);




	
	// left_lower_leg
	left_lower_leg->setNormalize(true);
	left_lower_leg->readObj("./assets/lower_leg.obj");

	// 设置物体的旋转位移
	//left_lower_leg->setTranslation(glm::vec3(-0.15, 0.15, 0.0));
	left_lower_leg->setTranslation(glm::vec3(0.0, 0.40, 0.0));
	left_lower_leg->setRotation(glm::vec3(0, 0, 0));
	left_lower_leg->setScale(glm::vec3(0.35, 0.35, 0.35));

	left_lower_leg->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
	left_lower_leg->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
	left_lower_leg->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
	left_lower_leg->setShininess(1.0); //高光系数

   // 加到painter中
	painter_left_lower_leg->addMesh(left_lower_leg, "mesh_a", "./assets/lower_leg.png", vshader, fshader);
	meshList3.push_back(left_lower_leg);




	
	// right_lower_leg
	right_lower_leg->setNormalize(true);
	right_lower_leg->readObj("./assets/lower_leg.obj");

	// 设置物体的旋转位移
	//right_lower_leg->setTranslation(glm::vec3(0.15, 0.15, 0.0));
	right_lower_leg->setTranslation(glm::vec3(0.0, 0.4, 0.0));
	right_lower_leg->setRotation(glm::vec3(0, 0, 0));
	right_lower_leg->setScale(glm::vec3(0.35, 0.35, 0.35));

	right_lower_leg->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
	right_lower_leg->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
	right_lower_leg->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
	right_lower_leg->setShininess(1.0); //高光系数

   // 加到painter中
	painter_right_lower_leg->addMesh(right_lower_leg, "mesh_a", "./assets/lower_leg.png", vshader, fshader);
	meshList3.push_back(right_lower_leg);
	



	TriMesh* cars = new TriMesh();
	// cars
	cars->setNormalize(true);
	cars->readObj("./assets/cars.obj");

	// 设置物体的旋转位移
	cars->setTranslation(glm::vec3(3, 0.5, 3.0));
	cars->setRotation(glm::vec3(0, 0, 0));
	cars->setScale(glm::vec3(3.0, 3.0, 3.0));

	cars->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
	cars->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
	cars->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
	cars->setShininess(1.0); //高光系数

   // 加到painter中
	painter->addMesh(cars, "mesh_a", "./assets/cars.png", vshader, fshader);
	meshList.push_back(cars);




	TriMesh* cars2 = new TriMesh();
	// car2
	cars2->setNormalize(true);
	cars2->readObj("./assets/cars2.obj");

	// 设置物体的旋转位移
	cars2->setTranslation(glm::vec3(3, 0.6, -5.0));
	cars2->setRotation(glm::vec3(0, 0, 0));
	cars2->setScale(glm::vec3(3.0, 3.0, 3.0));

	cars2->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
	cars2->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
	cars2->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
	cars2->setShininess(1.0); //高光系数

   // 加到painter中
	painter->addMesh(cars2, "mesh_a", "./assets/cars2.png", vshader, fshader);
	meshList.push_back(cars2);


	TriMesh* human1 = new TriMesh();
	//human1
	human1->setNormalize(true);
	human1->readObj("./assets/human1.obj");

	// 设置物体的旋转位移
	human1->setTranslation(glm::vec3(4, 0.9, -3.0));
	human1->setRotation(glm::vec3(0, 0, 0));
	human1->setScale(glm::vec3(3.0, 3.0, 3.0));

	human1->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
	human1->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
	human1->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
	human1->setShininess(1.0); //高光系数

   // 加到painter中
	painter->addMesh(human1, "mesh_a", "./assets/human1.png", vshader, fshader);
	meshList.push_back(human1);




	/*TriMesh* floor = new TriMesh();
	// floor
	floor->setNormalize(true);
	floor->readObj("./assets/floor.obj");

	// 设置物体的旋转位移
	floor->setTranslation(glm::vec3(0, -0.75, 0.0));
	floor->setRotation(glm::vec3(0, 0, 0));
	floor->setScale(glm::vec3(30.0, 3.0, 30.0));

	floor->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
	floor->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
	floor->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
	floor->setShininess(1.0); //高光系数

   // 加到painter中
	painter->addMesh(floor, "mesh_a", "./assets/floor.png", vshader, fshader);

	meshList.push_back(floor);*/


	TriMesh* house = new TriMesh();
	// house
	house->setNormalize(true);
	house->readObj("./assets/house.obj");

	// 设置物体的旋转位移
	house->setTranslation(glm::vec3(-7, 3.6, -7.0));
	house->setRotation(glm::vec3(0, 0, 0));
	house->setScale(glm::vec3(10.0, 10.0, 10.0));

	house->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
	house->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
	house->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
	house->setShininess(1.0); //高光系数

   // 加到painter中
	painter->addMesh(house, "mesh_a", "./assets/house.png", vshader, fshader);

	meshList.push_back(house);



	TriMesh* house2 = new TriMesh();
	// house2
	house2->setNormalize(true);
	house2->readObj("./assets/house2.obj");

	// 设置物体的旋转位移
	house2->setTranslation(glm::vec3(6, 3.925, 6.0));
	house2->setRotation(glm::vec3(0, 0, 0));
	house2->setScale(glm::vec3(10.0, 10.0, 10.0));

	house2->setAmbient(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 环境光
	house2->setDiffuse(glm::vec4(0.7, 0.7, 0.7, 1.0)); // 漫反射
	house2->setSpecular(glm::vec4(0.2, 0.2, 0.2, 1.0)); // 镜面反射
	house2->setShininess(1.0); //高光系数

   // 加到painter中
	painter->addMesh(house2, "mesh_a", "./assets/house2.png", vshader, fshader);

	meshList.push_back(house2);

	display_floor();

	glClearColor(0.0, 0.6, 1.0, 1.0);
	// glClearColor(0.0, 0.0, 0.0, 1.0);
}

//用于更新robot的位置
void renew_robot_dirction()
{
	//hat
	glm::vec3 original_dirction = hat->getTranslation();
	glm::vec3 last = original_dirction + glm::vec3(_x, _y, _z);
	hat->setTranslation(last);

	//head
	original_dirction = head->getTranslation();
	last = original_dirction + glm::vec3(_x, _y, _z);
	head->setTranslation(last);

	//body
	original_dirction = body->getTranslation();
	last = original_dirction + glm::vec3(_x, _y, _z);
	body->setTranslation(last);


	//left_upper_arm
	original_dirction = left_upper_arm->getTranslation();
	last = original_dirction + glm::vec3(_x, _y, _z);
	left_upper_arm->setTranslation(last);

	//right_upper_arm
	original_dirction = right_upper_arm->getTranslation();
	last = original_dirction + glm::vec3(_x, _y, _z);
	right_upper_arm->setTranslation(last);

	//left_lower_arm
	original_dirction = left_lower_arm->getTranslation();
	last = original_dirction + glm::vec3(_x, _y, _z);
	left_lower_arm->setTranslation(last);

	//right_lower_arm
	original_dirction = right_lower_arm->getTranslation();
	last = original_dirction + glm::vec3(_x, _y, _z);
	right_lower_arm->setTranslation(last);

	//left_upper_leg
	original_dirction = left_upper_leg->getTranslation();
	last = original_dirction + glm::vec3(_x, _y, _z);
	left_upper_leg->setTranslation(last);

	//right_upper_leg
	original_dirction = right_upper_leg->getTranslation();
	last = original_dirction + glm::vec3(_x, _y, _z);
	right_upper_leg->setTranslation(last);

	//left_lower_leg
	original_dirction = left_lower_leg->getTranslation();
	last = original_dirction + glm::vec3(_x, _y, _z);
	left_lower_leg->setTranslation(last);

	//right_lower_leg
	original_dirction = right_lower_leg->getTranslation();
	last = original_dirction + glm::vec3(_x, _y, _z);
	right_lower_leg->setTranslation(last);

	//重置_x、_y、_z
	_x = 0.0;
	_y = 0.0;
	_z = 0.0;
}


//用于更新robot的角度
void renew_robot_rotation()
{
	//hat
	glm::vec3 original_rotation = hat->getRotation();
	glm::vec3 last = original_rotation + glm::vec3(_X, _Y, _Z);
	hat->setRotation(last);
	

	//head
	original_rotation = head->getRotation();
	last = original_rotation + glm::vec3(_X, _Y, _Z);
	head->setRotation(last);

	//body
	original_rotation = body->getRotation();
	last = original_rotation + glm::vec3(_X, _Y, _Z);
	body->setRotation(last);


	//left_upper_arm
	original_rotation = left_upper_arm->getRotation();
	last = original_rotation + glm::vec3(_X, _Y, _Z);
	left_upper_arm->setRotation(last);

	//right_upper_arm
	original_rotation = right_upper_arm->getRotation();
	last = original_rotation + glm::vec3(_X, _Y, _Z);
	right_upper_arm->setRotation(last);

	//left_lower_arm
	original_rotation = left_lower_arm->getRotation();
	last = original_rotation + glm::vec3(_X, _Y, _Z);
	left_lower_arm->setRotation(last);

	//right_lower_arm
	original_rotation = right_lower_arm->getRotation();
	last = original_rotation + glm::vec3(_X, _Y, _Z);
	right_lower_arm->setRotation(last);

	//left_upper_leg
	original_rotation = left_upper_leg->getRotation();
	last = original_rotation + glm::vec3(_X, _Y, _Z);
	left_upper_leg->setRotation(last);

	//right_upper_leg
	original_rotation = right_upper_leg->getRotation();
	last = original_rotation + glm::vec3(_X, _Y, _Z);
	right_upper_leg->setRotation(last);

	//left_lower_leg
	original_rotation = left_lower_leg->getRotation();
	last = original_rotation + glm::vec3(_X, _Y, _Z);
	left_lower_leg->setRotation(last);

	//right_lower_leg
	original_rotation = right_lower_leg->getRotation();
	last = original_rotation + glm::vec3(_X, _Y, _Z);
	right_lower_leg->setRotation(last);

	//重置_X、_Y、_Z
	_X = 0.0;
	_Y = 0.0;
	_Z = 0.0;
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	painter->drawMeshes(light, camera);
	painter2->drawMeshes2(light, camera);
	display_robot();
	sky_painter->drawMeshes2(light, camera);
}


void printHelp()
{
	std::cout << "================================================" << std::endl;
	std::cout << "Use mouse to controll the light position (drag)." << std::endl;
	std::cout << "================================================" << std::endl << std::endl;

	std::cout << "Keyboard Usage" << std::endl;
	std::cout <<
		"[Window]" << std::endl <<
		"ESC:		Exit" << std::endl <<
		"h:		Print help message" << std::endl <<

		std::endl <<
		"[Camera]" << std::endl <<
		"SPACE:		Reset camera parameters" << std::endl <<
		"u/U:		Increase/Decrease the rotate angle" << std::endl <<
		"i/I:		Increase/Decrease the up angle" << std::endl <<
		"o/O:		Increase/Decrease the camera radius" << std::endl << std::endl;

}

//根据帧数，来绘制人物的移动动画
void display_move_status(int step)
{
	step = step % 100;
	if (step < 25)
	{
		theta[3] = 1.6 * step;
		theta[4] = -1.6 * step;
		theta[5] = 0.4 * step;
		theta[6] = -0.4 * step;
		theta[7] = -1.6 * step;
		theta[8] = 1.6 * step;
		theta[9] = -0.4 * step;
		theta[10] = 0.4 * step;
	}
	else if (step < 50)
	{
		theta[3] = 80-1.6 * step;
		theta[4] = -80+1.6 * step;
		theta[5] = 20-0.4 * step;
		theta[6] = -20+0.4 * step;
		theta[7] = -80+1.6 * step;
		theta[8] = 80-1.6 * step;
		theta[9] = -20+0.4 * step;
		theta[10] = 20-0.4 * step;
	}
	else if (step < 75)
	{
		theta[3] = -1.6 * (step-50);
		theta[4] = 1.6 * (step - 50);
		theta[5] = -0.4 * (step - 50);
		theta[6] = 0.4 * (step - 50);
		theta[7] = 1.6 * (step - 50);
		theta[8] = -1.6 * (step - 50);
		theta[9] = 0.4 * (step - 50);
		theta[10] = -0.4 * (step - 50);
	}
	else
	{
		theta[3] = -(80 - 1.6 * (step - 50));
		theta[4] = -(-80 + 1.6 * (step - 50));
		theta[5] = -(20 - 0.4 * (step - 50));
		theta[6] = -(-20 + 0.4 * (step - 50));
		theta[7] = -(-80 + 1.6 * (step - 50));
		theta[8] = -(80 - 1.6 * (step - 50));
		theta[9] = -(-20 + 0.4 * (step - 50));
		theta[10] = -(20 - 0.4 * (step - 50));
	}
}



int state1 = 0;
int step_state1 = 1;
//根据帧数，来更改body的关节角
void renew_theta(int step)
{
	if (step < 0)
	{
		step = -step;
		//step = step % 60;
		theta[2] = -step * 6;
	}
	else
	{
		//step = step % 60;
		theta[2] = step * 6;
	}
}

//当按下w时，更新位置变量
void renew_dirction_w()
{
	_x += 0.1f * glm::sin(glm::radians(theta[2]));
	_z += 0.1f * glm::cos(glm::radians(theta[2]));
}

//当按下s时，更新位置变量
void renew_dirction_s()
{
	_x -= 0.1f * glm::sin(glm::radians(theta[2]));
	_z -= 0.1f * glm::cos(glm::radians(theta[2]));
}

//当按下a时，更新位置变量
void renew_dirction_a()
{

}

//当按下d时，更新位置变量
void renew_dirction_d()
{

}

int step_state = 4;
// 键盘响应函数
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	float tmp;
	glm::vec4 ambient;
	if (true/*action == GLFW_PRESS*/) {
		switch (key)
		{
		case GLFW_KEY_ESCAPE: exit(EXIT_SUCCESS); break;
		case GLFW_KEY_H: printHelp(); break;
		case GLFW_KEY_SPACE:
			camera->keyboard(key, action, mode);
			break;
		/*case GLFW_KEY_UP:
			camera->keyboard(key, action, mode);
			break;
		case GLFW_KEY_LEFT:
			camera->keyboard(key, action, mode);
			break;
		case GLFW_KEY_RIGHT:
			camera->keyboard(key, action, mode);
			break;
		case GLFW_KEY_DOWN:
			camera->keyboard(key, action, mode);
			break;*/
		case GLFW_KEY_W:
			state+= step_state;
			display_move_status(state);
			//keyboard_matrix = glm::translate(keyboard_matrix, glm::vec3(0.0, 0.0, 1.1));
			//_z += 0.1f;
			renew_dirction_w();
			renew_robot_dirction();
				break;
		case GLFW_KEY_S:
			state+= step_state;
			display_move_status(state);
			//keyboard_matrix = glm::translate(keyboard_matrix, glm::vec3(0.0, 0.0, -1.1));
			//_z -= 0.1f;
			renew_dirction_s();
			renew_robot_dirction();
			break;
		case GLFW_KEY_A:
			state1 += step_state1;
			//theta[2] += 10.0f;
			//_x -= 0.1f;
			state+= step_state;
			renew_theta(state1);
			display_move_status(state);
			renew_robot_dirction();
			break;
		case GLFW_KEY_D:
			state1 -= step_state1;
			//theta[2] -= 10.0f;
			state+= step_state;
			//_x += 0.1f;
			renew_theta(state1);
			display_move_status(state);
			renew_robot_dirction();
			break;
		}
		
	}
	//std::cout << state << "  " << std::endl;
}

// 重新设置窗口
void reshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
}

void cleanData() {
	// 释放内存
	
	delete camera;
	camera = NULL;

	delete light;
	light = NULL;

	painter->cleanMeshes();

	delete painter;
	painter = NULL;
	
	painter2->cleanMeshes();
	delete painter2;
	painter2 = NULL;

	painter_hat->cleanMeshes();
	delete painter_hat;
	painter_hat = NULL;

	painter_head->cleanMeshes();
	delete painter_head;
	painter_head = NULL;

	painter_left_upper_arm->cleanMeshes();
	delete painter_left_upper_arm;
	painter_left_upper_arm = NULL;

	painter_right_upper_arm->cleanMeshes();
	delete painter_right_upper_arm;
	painter_right_upper_arm = NULL;

	painter_left_lower_arm->cleanMeshes();
	delete painter_left_lower_arm;
	painter_left_lower_arm = NULL;

	painter_right_lower_arm->cleanMeshes();
	delete painter_right_lower_arm;
	painter_right_lower_arm = NULL;

	painter_left_upper_leg->cleanMeshes();
	delete painter_left_upper_leg;
	painter_left_upper_leg = NULL;

	painter_right_upper_leg->cleanMeshes();
	delete painter_right_upper_leg;
	painter_right_upper_leg = NULL;

	painter_left_lower_leg->cleanMeshes();
	delete painter_left_lower_leg;
	painter_left_lower_leg = NULL;

	painter_right_lower_leg->cleanMeshes();
	delete painter_right_lower_leg;
	painter_right_lower_leg = NULL;

	for (int i=0; i<meshList.size(); i++) {
		delete meshList[i];
	}
	meshList.clear();

	for (int i = 0; i < meshList2.size(); i++) {
		delete meshList2[i];
	}
	meshList2.clear();

	for (int i = 0; i < meshList3.size(); i++)
	{
		delete meshList3[i];
	}
	meshList3.clear();
}
//通过鼠标控制视角函数
void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	camera->mousePos(xpos, ypos);
}

//通过鼠标滚轮来控制z
void sroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera->sroll(xoffset, yoffset);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main(int argc, char **argv)
{
	// 初始化GLFW库，必须是应用程序调用的第一个GLFW函数
	glfwInit();

	// 配置GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// 配置窗口属性
	GLFWwindow* window = glfwCreateWindow(600, 600, "YangYi-2019152080", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, sroll_callback);
	glfwSetCursorPosCallback(window, mouse_pos_callback);

	// 调用任何OpenGL的函数之前初始化GLAD
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Init mesh, shaders, buffer
	init();
	// 输出帮助信息
	printHelp();
	// 启用深度测试
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		display();
		//reshape();

		// 交换颜色缓冲 以及 检查有没有触发什么事件（比如键盘输入、鼠标移动等）
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	cleanData();


	return 0;
}

// 每当窗口改变大小，GLFW会调用这个函数并填充相应的参数供你处理。
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow * window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
