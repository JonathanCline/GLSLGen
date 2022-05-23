ADD_CMAKE_SUBDIRS_HERE()


ADD_GIT_DEPENDENCY("${CMAKE_CURRENT_LIST_DIR}/glfw" glfw "https://github.com/glfw/glfw")
ADD_GIT_DEPENDENCY("${CMAKE_CURRENT_LIST_DIR}/jclib" jclib "https://github.com/JonathanCline/JCLib" develop)
ADD_GIT_DEPENDENCY("${CMAKE_CURRENT_LIST_DIR}/cppopengl" cppopengl "https://github.com/JonathanCline/cppopengl")
