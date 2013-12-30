#<make-shader "shader"
#:vertex-shader #{
#version 150 core
		in vec3 in_pos;
		uniform mat4 proj;
		uniform mat4 view;
		
		void main() {
			vec4 pos = proj * view * vec4(in_pos, 1);
			gl_Position = vec4(pos);
		}
}
#:fragment-shader #{
#version 150 core
	out vec4 out_col;
	void main() {
                out_col = vec4(0,0,0,1);
	}
}
#:inputs (list "in_pos")>

