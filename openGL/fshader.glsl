// Per-fragment interpolated values from the vertex shader

//Since normals are fixed for a given face of the cube in this example, fN per-fragment interpolation yields fixed values. Per-fragment interpolation of fL and fV however gives smoothly varying values through faces.

varying  vec3 fN;
varying  vec3 fL;
varying  vec3 fV;
varying vec4 color;
varying  vec2 texCoord;
uniform int TextureFlag;

uniform sampler2D texture;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform mat4 ModelView;
uniform vec4 LightPosition;
uniform float Shininess;

uniform int type;
uniform int shading;

void main() 
{ 
	if (type == 1){
		
			// Normalize the input lighting vectors
			vec3 N = normalize(fN);
			vec3 V = normalize(fV);
			vec3 L = normalize(fL);

			vec3 H;
			float Ks;
			vec4 specular;

			if(shading == 1){
				H = normalize( L + V );
				Ks = pow(max(dot(N, H), 0.0), Shininess);
				specular = Ks*SpecularProduct;
			}else{
				H = reflect( -L, N );
				Ks = pow(max(dot(H, V), 0.0), Shininess);
				specular = Ks*SpecularProduct;
			}
			vec4 ambient = AmbientProduct;
	
			float Kd = max(dot(L, N), 0.0);
			vec4 diffuse = Kd*DiffuseProduct;
        
			
	
			// discard the specular highlight if the light's behind the vertex
			if( dot(L, N) < 0.0 ) {
			    specular = vec4(0.0, 0.0, 0.0, 1.0);
			}
		if (TextureFlag == 1){
			gl_FragColor = texture( texture, texCoord ) + ambient + diffuse + specular + color;
			gl_FragColor.a = 1.0;
		}else{
			gl_FragColor = ambient + diffuse + specular + color;
			gl_FragColor.a = 1.0;
		}
	}else{
		if (TextureFlag == 1){
        gl_FragColor = texture2D( texture, texCoord );
		}else {
			gl_FragColor = color;
		}
	}
} 

