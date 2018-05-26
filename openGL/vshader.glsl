attribute  vec4 vPosition;
attribute  vec3 vNormal;
varying vec4 color;
attribute  vec4 vColor;
attribute  vec2 vTexCoord;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform mat4 ModelView;
uniform vec4 LightPosition;
uniform mat4 Projection;
uniform float Shininess;
varying vec2 texCoord;

// output values that will be interpretated per-fragment
varying  vec3 fN;
varying  vec3 fV;
varying  vec3 fL;

uniform int type;
uniform int shading;

void main()
{
	if(type == 1 ){
	    // Transform vertex position into camera (eye) coordinates
	    vec3 pos = (ModelView * vPosition).xyz;
    
	    fN = (ModelView * vec4(vNormal, 0.0)).xyz; // normal direction in camera coordinates

	    fV = -pos; //viewer direction in camera coordinates

	    fL = LightPosition.xyz; // light direction

	    if( LightPosition.w != 0.0 ) {
	        fL = LightPosition.xyz - pos;  //directional light source
	    }
    
	    gl_Position = Projection * ModelView * vPosition;
		color = vColor;
		texCoord    = vTexCoord;
	}else{
		vec3 L;
		vec3 pos = (ModelView * vPosition).xyz;
		if( LightPosition.w != 0.0 ) {
	        L = LightPosition.xyz - pos;  //directional light source
	    }else{
			L = normalize( LightPosition.xyz ); //light direction
		}
		vec3 V = normalize( -pos ); // viewer direction
		vec3 N = normalize( ModelView * vec4(vNormal, 0.0) ).xyz;
		vec3 H;
		float Ks;
		vec4  specular;
		if(shading == 1){
			H = normalize( L + V ); // halfway vector
			Ks = pow( max(dot(N, H), 0.0), Shininess );
			specular = Ks * SpecularProduct;
		}else{
			H = reflect( -L, N );
			Ks = pow(max(dot(H, V), 0.0), Shininess);
			specular = Ks*SpecularProduct;
		}
	    // Compute terms in the illumination equation
	    vec4 ambient = AmbientProduct;
	
	    float Kd = max( dot(L, N), 0.0 ); //set diffuse to 0 if light is behind the surface point
	    vec4  diffuse = Kd*DiffuseProduct;

	    
    
	    //ignore also specular component if light is behind the surface point
	    if( dot(L, N) < 0.0 ) {
	        specular = vec4(0.0, 0.0, 0.0, 1.0);
	    } 

	    gl_Position = Projection * ModelView * vPosition;
		texCoord    = vTexCoord;

	    color = ambient + diffuse + specular + vColor;
	    color.a = 1.0;
	}
}
