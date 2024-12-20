CXX = g++
CXXFLAGS = -std=c++17 -Wall -I/usr/include -I/usr/local/include -I/usr/include/glm -I./stbImage -I/usr/include/assimp
LDFLAGS = -lglfw -lGL -lGLEW -lassimp

SRC =  main.cpp Mortar.cpp  Camera.cpp Model.cpp Skybox.cpp HeightmapTerrain.cpp Character.cpp Frustum.cpp Shaders.cpp Skybox.cpp ObjectBase.cpp Animation.cpp Skeleton.cpp
OBJ = $(SRC:.cpp=.o) 
EXEC = mortar

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)

