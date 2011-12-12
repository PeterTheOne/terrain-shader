

void main() {			
	// Set the position of the current vertex 
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}