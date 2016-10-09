#include "Model.hpp"

namespace Helix {
ModelLoader::ModelLoader() {}

ModelLoader::~ModelLoader() {
	if(textureSurface) {
		SDL_FreeSurface(textureSurface);
	}
}

void ModelLoader::LoadModel(std::string fileName, Model* m) {
	Assimp::Importer importer;

	// set maximum 4 bones per vertex (4 is also default value)
	//importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 4);

	const aiScene* scene = importer.ReadFile(fileName,
	
	                       aiProcess_Triangulate |
	                       aiProcess_OptimizeMeshes |
	                       aiProcess_JoinIdenticalVertices |
	                       //  aiProcess_SplitLargeMeshes |
	                       //aiProcess_PreTransformVertices |
	                       aiProcess_LimitBoneWeights |
	                       aiProcess_GenNormals |
	                       // aiProcess_GenSmoothNormals |
	                       aiProcess_FlipUVs);

	if(!scene) {
		throw std::string("Assimp error: ") + importer.GetErrorString();
	}

	m->rootPath = fileName;
	for(int x = m->rootPath.size() - 1; x >= 0; x--) {
		if(m->rootPath[x] == '/' || m->rootPath[x] == '\\') {
			m->rootPath = m->rootPath.substr(0,  x + 1);
			break;
		}
	}

	// some debug stuff, delete later
	//std::cout << "Number of total meshes: " << scene->mNumMeshes << std::endl;
	//std::cout << "Animations: " << scene->HasAnimations() << std::endl;

	// set 64 bones to identity, 64 is current limit, might increase it later
	processAnimations(scene, m);

	// start processing the model
	processNode(scene, scene->mRootNode, m);

	// must be called after processNode
	if(scene->HasAnimations()) {
		m->animations[m->currentAnim].buildBoneTree(scene, scene->mRootNode, &m->animations[m->currentAnim].rootBone, m);
	}

	m->modelTrans = glm::mat4(1.0f);
	m->modelLoaded = true;

	m->init();
}

void ModelLoader::processAnimations(const aiScene* scene, Model* m) {
	if(scene->HasAnimations()) {
		for(int x = 0; x < scene->mNumAnimations; x++) {
			auto &anim = scene->mAnimations[x];
			Model::Animation tempAnim;
			tempAnim.rootBone.parent = 0;
			tempAnim.name = anim->mName.data;
			tempAnim.duration = anim->mDuration;
			tempAnim.ticksPerSecond = anim->mTicksPerSecond;

			// load in required data for animation so that we don't have to save the entire scene
			for(int y = 0; y < anim->mNumChannels; y++) {
				Model::Animation::Channel tempChan;
				tempChan.name = anim->mChannels[y]->mNodeName.data;

				for(int z = 0; z < anim->mChannels[y]->mNumPositionKeys; z++) {
					tempChan.positionKeys.push_back(anim->mChannels[y]->mPositionKeys[z]);
				}

				for (int z = 0; z < anim->mChannels[y]->mNumRotationKeys; z++) {
					tempChan.rotationKeys.push_back(anim->mChannels[y]->mRotationKeys[z]);
				}

				for (int z = 0; z < anim->mChannels[y]->mNumScalingKeys; z++) {
					tempChan.scalingKeys.push_back(anim->mChannels[y]->mScalingKeys[z]);
				}

				tempAnim.channels.push_back(tempChan);
			}

			m->currentAnim = 0;

			tempAnim.boneTrans.reserve(MAX_BONES);
			for(int z = 0; z < MAX_BONES; z++) {
				tempAnim.boneTrans.push_back(glm::mat4(1.0f));
			}

			m->animations.push_back(tempAnim);
		}

		m->animations[m->currentAnim].rootBone.name = "rootBoneTreeNode";
	}
}

void ModelLoader::processNode(const aiScene* scene, aiNode* n, Model* m) {
	std::cout << "Processing a node: " << n->mName.C_Str() << std::endl; //debug

	// cycle through each mesh within this node
	if(n->mNumMeshes > 0) {
		// cycle through each mesh
		for(unsigned int x = 0; x < n->mNumMeshes; x++) {
			unsigned int mesh_id = n->mMeshes[x];
			processMesh(scene, n, scene->mMeshes[mesh_id], m);
		}
	}

	/*
	if(m->boneID.find(node->mName.data) != m->boneID.end()) {
	    std::cout << node->mName.data << " IS A BONE NODE!!!!";
	}
	*/

	// then go through each child in the node and process their meshes as well
	if(n->mNumChildren > 0) {
		for(unsigned int x = 0; x < n->mNumChildren; x++) {
			processNode(scene, n->mChildren[x], m);
		}
	}
}

// add some error handling (not all models have uvs, etc)
void ModelLoader::processMesh(const aiScene* scene, aiNode* node, aiMesh* mesh, Model* m) {
	std::cout << "Processing a mesh: " << mesh->mName.C_Str() << std::endl;
	std::cout << "Has bones? " << mesh->mNumBones << std::endl;

	Model::Mesh tempMesh;

	tempMesh.baseModelMatrix = toMat4(&node->mTransformation);
	if(node->mParent != NULL) {
		tempMesh.baseModelMatrix = toMat4(&node->mParent->mTransformation) * toMat4(&node->mTransformation);
	}

	// loop through each vertex in the mesh
	for(unsigned x = 0; x < mesh->mNumVertices; x++) {
		// load the vertices
		glm::vec3 tempV;
		tempV.x = mesh->mVertices[x].x;
		tempV.y = mesh->mVertices[x].y;
		tempV.z = mesh->mVertices[x].z;
		tempMesh.vertices.push_back(tempV);

		// bounding box
		if(mesh->mVertices[x].x < m->m_boundingBoxMin.x) m->m_boundingBoxMin.x = mesh->mVertices[x].x;
		if(mesh->mVertices[x].y < m->m_boundingBoxMin.y) m->m_boundingBoxMin.y = mesh->mVertices[x].y;
		if(mesh->mVertices[x].z < m->m_boundingBoxMin.z) m->m_boundingBoxMin.z = mesh->mVertices[x].z;
		if(mesh->mVertices[x].x > m->m_boundingBoxMax.x) m->m_boundingBoxMax.x = mesh->mVertices[x].x;
		if(mesh->mVertices[x].y > m->m_boundingBoxMax.y) m->m_boundingBoxMax.y = mesh->mVertices[x].y;
		if(mesh->mVertices[x].z > m->m_boundingBoxMax.z) m->m_boundingBoxMax.z = mesh->mVertices[x].z;

		// load the first uvs (if they exist)
		if(mesh->mTextureCoords[0]) {
			glm::vec2 tempUV;
			tempUV.x = mesh->mTextureCoords[0][x].x;
			tempUV.y = mesh->mTextureCoords[0][x].y;
			tempMesh.uvs.push_back(tempUV);
		}

		// load the normals (if they exist)
		if(mesh->HasNormals()) {
			glm::vec3 tempN;
			tempN.x = mesh->mNormals[x].x;
			tempN.y = mesh->mNormals[x].y;
			tempN.z = mesh->mNormals[x].z;
			tempMesh.normals.push_back(tempN);
		}
	}

	// cycle through each face to get the indices
	for(unsigned int x = 0; x < mesh->mNumFaces; x++) {
		// ALWAYS USE AIPROCESS_TRIANGULATE!!! not doing so will make all of the indices wrong!!!
		tempMesh.indices.push_back(mesh->mFaces[x].mIndices[0]);
		tempMesh.indices.push_back(mesh->mFaces[x].mIndices[1]);
		tempMesh.indices.push_back(mesh->mFaces[x].mIndices[2]);
	}

	if(scene->HasMaterials()) {
		aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
		//std::cout << "Has diffuse texture: " << mat->GetTextureCount(aiTextureType_DIFFUSE) << std::endl;
		//std::cout << "Has specular texture: " << mat->GetTextureCount(aiTextureType_SPECULAR) << std::endl;

		// add optimization to not to load textures, those were already loaded, store in a vector in ModelLoader,
		// so it holds all textures withing different models
		if(mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString texturePath;
			mat->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
			//std::cout << texturePath.C_Str() << std::endl;

			std::string textureFileName = m->rootPath + texturePath.C_Str();

			//in case some model has path textures\filename.jpg
			std::replace(textureFileName.begin(), textureFileName.end(), '\\', '/');

			textureSurface = IMG_Load(textureFileName.c_str());
			if(!textureSurface) {
				throw std::string("Error loading image: ") + IMG_GetError();
			}

			tempMesh.image = textureSurface;

			// free surface after binding texture to opengl? (cleanup destructor then)
		}
	}

	if(scene->HasAnimations()) {
		tempMesh.hasAnimation = true;
	} else {
		tempMesh.hasAnimation = false;
	}

	if(mesh->HasBones()) {
		tempMesh.weights.resize(mesh->mNumVertices);
		std::fill(tempMesh.weights.begin(), tempMesh.weights.end(), glm::vec4(0.0f));
		tempMesh.boneID.resize(mesh->mNumVertices);
		std::fill(tempMesh.boneID.begin(), tempMesh.boneID.end(), glm::vec4(0.0f));

		for(int x = 0; x < mesh->mNumBones; x++) {
			// bone index, decides what bone we modify
			unsigned int index = 0;

			// bone name -> bone index
			if(m->boneID.find(mesh->mBones[x]->mName.data) == m->boneID.end()) {
				// create a new bone
				// current index is the new bone
				index = m->boneID.size();
			} else {
				index = m->boneID[mesh->mBones[x]->mName.data];
			}
			m->boneID[mesh->mBones[x]->mName.data] = index;

			// loading offset matrices
			for(int y = 0; y < m->animations[m->currentAnim].channels.size(); y++) {
				if (m->animations[m->currentAnim].channels[y].name == mesh->mBones[x]->mName.data)
					m->animations[m->currentAnim].boneOffset[mesh->mBones[x]->mName.data] = toMat4(&mesh->mBones[x]->mOffsetMatrix);
			}

			
			for(int y = 0; y < mesh->mBones[x]->mNumWeights; y++) {
				unsigned int vertexID = mesh->mBones[x]->mWeights[y].mVertexId;
				// first we check if the boneid vector has any filled in
				// if it does then we need to fill the weight vector with the same value
				if(tempMesh.boneID[vertexID].x == 0) {
					tempMesh.boneID[vertexID].x = index;
					tempMesh.weights[vertexID].x = mesh->mBones[x]->mWeights[y].mWeight;
				} else if(tempMesh.boneID[vertexID].y == 0) {
					tempMesh.boneID[vertexID].y = index;
					tempMesh.weights[vertexID].y = mesh->mBones[x]->mWeights[y].mWeight;
				} else if(tempMesh.boneID[vertexID].z == 0) {
					tempMesh.boneID[vertexID].z = index;
					tempMesh.weights[vertexID].z = mesh->mBones[x]->mWeights[y].mWeight;
				} else if(tempMesh.boneID[vertexID].w == 0) {
					tempMesh.boneID[vertexID].w = index;
					tempMesh.weights[vertexID].w = mesh->mBones[x]->mWeights[y].mWeight;
				}
			}

		}
	}

	m->meshes.push_back(tempMesh);
}

void Model::Animation::buildBoneTree(const aiScene* scene, aiNode* node, BoneNode* bNode, Model* m) {
	if(scene->HasAnimations()) {
		// found the node
		if(m->boneID.find(node->mName.data) != m->boneID.end()) {
			//std::cout << "Found a bone node: " << node->mName.data << std::endl;

			BoneNode tempNode;
			tempNode.name = node->mName.data;
			tempNode.parent = bNode;
			tempNode.nodeTransform = toMat4(&node->mTransformation);
			// bones and their nodes always share the same name
			tempNode.boneTransform = boneOffset[tempNode.name];
			bNode->children.push_back(tempNode);
		}

		if(node->mNumChildren > 0) {
			for(unsigned int x = 0; x < node->mNumChildren; x++) {
				// if the node we just found was a bone node then pass it in (current bone node child vector size - 1)
				if(m->boneID.find(node->mName.data) != m->boneID.end()) {
					buildBoneTree(scene, node->mChildren[x], &bNode->children[bNode->children.size() - 1], m);
				} else {
					buildBoneTree(scene, node->mChildren[x], bNode, m);
				}
			}
		}
	}
}

glm::vec3 Model::GetBoundingBoxMin() const {
	return m_boundingBoxMin;
}

glm::vec3 Model::GetBoundingBoxMax() const {
	return m_boundingBoxMax;
}

// there is a bug with interpolation
void Model::SetTick(double time) {
	double timeInTicks = time * animations[currentAnim].ticksPerSecond;

	updateBoneTree(timeInTicks, &animations[currentAnim].rootBone, glm::mat4(1.0f));
}

void Model::updateBoneTree(double timeInTicks, Model::Animation::BoneNode* node, glm::mat4 parentTransform) {
	int chanIndex = 0;
	for(int x = 0; x < animations[currentAnim].channels.size(); x++) {
		if(node->name == animations[currentAnim].channels[x].name) {
			chanIndex = x;
		}
	}

	double animTime = std::fmod(timeInTicks, animations[currentAnim].duration);

	aiQuaternion rotation(animations[currentAnim].channels[chanIndex].rotationKeys[0].mValue);
	aiVector3D translation(animations[currentAnim].channels[chanIndex].positionKeys[0].mValue);
	aiVector3D scale(animations[currentAnim].channels[chanIndex].scalingKeys[0].mValue);

	Assimp::Interpolator<aiQuaternion> slerp;
	Assimp::Interpolator<aiVector3D> lerp;

	// get the two animation keys it is between for lerp and slerp
	int key1, key2;
	if(std::round(animTime) < animTime) {
		key1 = std::round(animTime);
		key2 = key1 + 1;
	} else {
		key1 = std::round(animTime) - 1;
		key2 = std::round(animTime);
	}

	// translation
	if(animations[currentAnim].channels[chanIndex].positionKeys.size() > 1) {
		lerp(translation, animations[currentAnim].channels[chanIndex].positionKeys[key1].mValue, 
						  animations[currentAnim].channels[chanIndex].positionKeys[key2].mValue, 
						  animTime - key1); 
	}

	// scale
	if(animations[currentAnim].channels[chanIndex].scalingKeys.size() > 1) {
		lerp(scale, animations[currentAnim].channels[chanIndex].scalingKeys[key1].mValue, 
					animations[currentAnim].channels[chanIndex].scalingKeys[key2].mValue, 
					animTime - key1); 
	}

	// rotation
	if(animations[currentAnim].channels[chanIndex].rotationKeys.size() > 1) {
		slerp(rotation, animations[currentAnim].channels[chanIndex].rotationKeys[key1].mValue, 
						animations[currentAnim].channels[chanIndex].rotationKeys[key2].mValue, 
						animTime - key1); 
	}

	glm::vec3 vtranslation((GLfloat)translation.x, (GLfloat)translation.y, (GLfloat)translation.z);
	glm::vec3 vscaling((GLfloat)scale.x, (GLfloat)scale.y, (GLfloat)scale.z);
	glm::quat vrotation((GLfloat)rotation.w, (GLfloat)rotation.x, (GLfloat)rotation.y, (GLfloat)rotation.z);

	// apply transforms
	glm::mat4 finalModel = parentTransform
	                       * glm::translate(glm::mat4(1.0f), vtranslation)
	                       * glm::mat4_cast(vrotation)
	                       * glm::scale(glm::mat4(1.0f), vscaling);

	animations[currentAnim].boneTrans[boneID[node->name]] = finalModel * animations[currentAnim].boneOffset[node->name];

	// loop through every child and use this bone's transformations as the parent transform
	for(int x = 0; x < node->children.size(); x++) {
		updateBoneTree(timeInTicks, &node->children[x], finalModel);
	}
}

Model::Model() {}

Model::Model(GLuint shader) : modelLoaded(false) {
	m_shader = shader;
}

Model::~Model() {}

GLint get_color_mode(SDL_Surface* surf) {
	GLint colorMode;
	if(surf->format->BytesPerPixel == 4) {
		if(surf->format->Rmask == 0x000000ff) {
			colorMode = GL_RGBA;
		} else {
			colorMode = GL_BGRA;
		}
	} else if(surf->format->BytesPerPixel == 3) {
		if(surf->format->Rmask == 0x000000ff) {
			colorMode = GL_RGB;
		} else {
			colorMode = GL_BGR;
		}
	} else {
		throw std::string("Image is not truecolor!");
	}
	return colorMode;
}

void Model::init() {
	if(!modelLoaded) {
		throw std::string("Please load in a model before initializing buffers.");
	}

	//std::cout << "meshes.size(): " << meshes.size() << std::endl;

	// loop through each mesh and initialize them
	for(int x = 0; x < meshes.size(); x++) {
		GLint colorMode = get_color_mode( meshes[x].image );

		glGenVertexArrays(1, &meshes[x].vao);
		glBindVertexArray(meshes[x].vao);

		glGenBuffers(1, &meshes[x].vbo);
		glBindBuffer(GL_ARRAY_BUFFER, meshes[x].vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * meshes[x].vertices.size(), &meshes[x].vertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &meshes[x].nbo);
		glBindBuffer(GL_ARRAY_BUFFER, meshes[x].nbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * meshes[x].normals.size(), &meshes[x].normals[0], GL_STATIC_DRAW);

		glGenBuffers(1, &meshes[x].ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes[x].ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * meshes[x].indices.size(), &meshes[x].indices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &meshes[x].uvb);
		glBindBuffer(GL_ARRAY_BUFFER, meshes[x].uvb);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * meshes[x].uvs.size(), &meshes[x].uvs[0], GL_STATIC_DRAW);

		glGenBuffers(1, &meshes[x].wbo);
		glBindBuffer(GL_ARRAY_BUFFER, meshes[x].wbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * meshes[x].weights.size(), &meshes[x].weights[0], GL_STATIC_DRAW);

		glGenBuffers(1, &meshes[x].idbo);
		glBindBuffer(GL_ARRAY_BUFFER, meshes[x].idbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * meshes[x].boneID.size(), &meshes[x].boneID[0], GL_STATIC_DRAW);

		{ // load textures
			glEnable(GL_TEXTURE_2D);
			glGenTextures(1, &meshes[x].tex);
			glBindTexture(GL_TEXTURE_2D, meshes[x].tex);

			bool lock = SDL_MUSTLOCK(meshes[x].image);
			if(lock) {
				SDL_LockSurface(meshes[x].image);
			}

			// or GL_BGR instead of colorMode
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, meshes[x].image->w, meshes[x].image->h, 0, colorMode, GL_UNSIGNED_BYTE, meshes[x].image->pixels);

			if(lock) {
				SDL_UnlockSurface(meshes[x].image);
			}
			// glBindTexture(GL_TEXTURE_2D, 0);

			//generate texture, bind and upload via glTexImage2D only on unique, lets say 15 texture files, not on 300+ meshes, which share these 15 textures

			// if(x < 320)
				// glDeleteTextures(1, &meshes[x].tex);
		}

		// tex data bound to uniform
		// not needed?
		//glUniform1i(glGetUniformLocation(m_shader, "texture_diffuse"), 0);
		glUseProgram(m_shader);
		
		glBindBuffer(GL_ARRAY_BUFFER, meshes[x].vbo);
		meshes[x].posAttribute = glGetAttribLocation(m_shader, "position");
		glEnableVertexAttribArray(meshes[x].posAttribute);
		glVertexAttribPointer(meshes[x].posAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, meshes[x].nbo);
		meshes[x].normalAttribute = glGetAttribLocation(m_shader, "normal");
		glEnableVertexAttribArray(meshes[x].normalAttribute);
		glVertexAttribPointer(meshes[x].normalAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);

		// now send uv data
		glBindBuffer(GL_ARRAY_BUFFER, meshes[x].uvb);
		meshes[x].texAttribute = glGetAttribLocation(m_shader, "texCoords");
		glEnableVertexAttribArray(meshes[x].texAttribute);
		glVertexAttribPointer(meshes[x].texAttribute, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, meshes[x].wbo);
		meshes[x].weightAttribute = glGetAttribLocation(m_shader, "weight");
		glEnableVertexAttribArray(meshes[x].weightAttribute);
		glVertexAttribPointer(meshes[x].weightAttribute, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, meshes[x].idbo);
		meshes[x].boneAttribute = glGetAttribLocation(m_shader, "boneID");
		glEnableVertexAttribArray(meshes[x].boneAttribute);
		// glVertexAttribPointer(meshes[x].boneAttribute, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribIPointer(meshes[x].boneAttribute, 4, GL_INT, 0, 0);

		meshes[x].modelID = glGetUniformLocation(m_shader, "model");
		meshes[x].viewID = glGetUniformLocation(m_shader, "view");
		meshes[x].projectionID = glGetUniformLocation(m_shader, "projection");
		
		meshes[x].boneTransformationID = glGetUniformLocation(m_shader, "boneTransformation");
		meshes[x].modelTransformID = glGetUniformLocation(m_shader, "modelTransform");

		glBindVertexArray(0);
	}

	m_boundingBoxVertices[0] = glm::vec3(this->GetBoundingBoxMin().x, this->GetBoundingBoxMin().y, this->GetBoundingBoxMin().z);
	m_boundingBoxVertices[1] = glm::vec3(this->GetBoundingBoxMin().x, this->GetBoundingBoxMin().y, this->GetBoundingBoxMax().z);
	m_boundingBoxVertices[2] = glm::vec3(this->GetBoundingBoxMin().x, this->GetBoundingBoxMax().y, this->GetBoundingBoxMin().z);
	m_boundingBoxVertices[3] = glm::vec3(this->GetBoundingBoxMin().x, this->GetBoundingBoxMax().y, this->GetBoundingBoxMax().z);
	m_boundingBoxVertices[4] = glm::vec3(this->GetBoundingBoxMax().x, this->GetBoundingBoxMin().y, this->GetBoundingBoxMin().z);
	m_boundingBoxVertices[5] = glm::vec3(this->GetBoundingBoxMax().x, this->GetBoundingBoxMin().y, this->GetBoundingBoxMax().z);
	m_boundingBoxVertices[6] = glm::vec3(this->GetBoundingBoxMax().x, this->GetBoundingBoxMax().y, this->GetBoundingBoxMin().z);
	m_boundingBoxVertices[7] = glm::vec3(this->GetBoundingBoxMax().x, this->GetBoundingBoxMax().y, this->GetBoundingBoxMax().z);
}

void Model::Draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection) {
	this->drawModel(model, view, projection, m_shader);
}

void Model::Draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection, GLuint shader) {
	this->drawModel(model, view, projection, shader);
}

// this is just a generic render function for quick and easy rendering
void Model::drawModel(glm::mat4 model, glm::mat4 view, glm::mat4 projection, GLuint shader) {
	if(!modelLoaded) {
		throw std::string("Model could not be rendered.");
	}


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glUseProgram(shader);
	for(int x = 0; x < meshes.size(); x++) {
		glBindVertexArray(meshes[x].vao);

		// &meshes[x].baseModelMatrix[0][0]
		glUniformMatrix4fv(meshes[x].modelID, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(meshes[x].viewID, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(meshes[x].projectionID, 1, GL_FALSE, glm::value_ptr(projection));

		// clean hasAnimation
		//GLint hasAnimation = glGetUniformLocation(m_shader, "hasAnimation");
		if(meshes[x].hasAnimation) {
			glUniformMatrix4fv(meshes[x].boneTransformationID, animations[currentAnim].boneTrans.size(), GL_FALSE, (GLfloat*)&animations[currentAnim].boneTrans[0][0]);
			glUniformMatrix4fv(meshes[x].modelTransformID, 1, GL_FALSE, (GLfloat*)&modelTrans[0][0]);

			// glUniform1i(hasAnimation, true);
		}
		//else {
			// glUniform1i(hasAnimation, false);
		//}

		glm::vec3 lightPos(2.7f, 0.2f, 2.0f);
		glm::vec3 lightColor(1.0f, 5.0f, 1.0f);

		GLint lightPosLoc = glGetUniformLocation(shader, "lightPos");
		GLint lightColorLoc = glGetUniformLocation(shader, "lightColor");

		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(lightColorLoc, lightColor.x, lightColor.y, lightColor.z);

		// glBindBuffer(GL_ARRAY_BUFFER, meshes[x].vbo);
		// glEnableVertexAttribArray(meshes[x].posAttribute);
		// glVertexAttribPointer(meshes[x].posAttribute, 3, GL_FLOAT, GL_FALSE, 0, 0);

		// glBindBuffer(GL_ARRAY_BUFFER, meshes[x].uvb);
		// glEnableVertexAttribArray(meshes[x].texAttribute);
		// glVertexAttribPointer(meshes[x].texAttribute, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, meshes[x].tex);
		glUniform1i(glGetUniformLocation(shader, "texture_diffuse1"), 0);
		
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glDrawElements(GL_TRIANGLES, meshes[x].indices.size(), GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glUseProgram(0);
}

std::vector<glm::vec3> Model::GetBoundingBoxVertices() {
	std::vector<glm::vec3> bbox(m_boundingBoxVertices.begin(), m_boundingBoxVertices.end());
	return bbox;
}

void Model::DrawBoundingBox(glm::mat4 model, glm::mat4 view, glm::mat4 projection, GLuint shader) {
	GLubyte indices[] = {0, 1, 1, 5, 5, 4, 4, 0,
	                     2, 3, 3, 7, 7, 6, 6, 2,
	                     0, 2, 1, 3, 5, 7, 4, 6,
	                    };

	GLfloat colors[] = {1.0, 1.0, 1.0,
	                    1.0, 1.0, 1.0,
	                    1.0, 1.0, 1.0,
	                    1.0, 1.0, 1.0,
	                    1.0, 1.0, 1.0,
	                    1.0, 1.0, 1.0,
	                    1.0, 1.0, 1.0,
	                    1.0, 1.0, 1.0,
	                   };

	glUseProgram(shader);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint vao_0, vbo_0, vbo_1;

	glGenVertexArrays(1, &vao_0);
	glGenBuffers(1, &vbo_0);
	glGenBuffers(1, &vbo_1);

	glBindVertexArray(vao_0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * this->GetBoundingBoxVertices().size(), &this->GetBoundingBoxVertices()[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	//glDrawArrays(GL_POINTS, 0, 8);
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_BYTE, indices);

	glBindVertexArray(0);

	glDeleteVertexArrays(1, &vao_0);
	glDeleteBuffers(1, &vbo_0);
	glDeleteBuffers(1, &vbo_1);

	glDisable(GL_PROGRAM_POINT_SIZE);
	glDisable(GL_BLEND);

	glUseProgram(0);
}

void Model::SetModelTrans(glm::mat4 in) {
	modelTrans = in;
}

glm::mat4 toMat4(aiMatrix4x4* ai) {
	glm::mat4 mat;

	mat[0][0] = ai->a1;
	mat[1][0] = ai->a2;
	mat[2][0] = ai->a3;
	mat[3][0] = ai->a4;
	
	mat[0][1] = ai->b1;
	mat[1][1] = ai->b2;
	mat[2][1] = ai->b3;
	mat[3][1] = ai->b4;
	
	mat[0][2] = ai->c1;
	mat[1][2] = ai->c2;
	mat[2][2] = ai->c3;
	mat[3][2] = ai->c4;
	
	mat[0][3] = ai->d1;
	mat[1][3] = ai->d2;
	mat[2][3] = ai->d3;
	mat[3][3] = ai->d4;

	return mat;
}
}
