/**
 * OpenGL Shader Collection
 * GLSL Shaders for advanced graphics pipeline
 */

export const GLShaders = {
  // Phong lighting model vertex shader
  phongVertex: `
    #version 300 es
    precision highp float;

    // Input vertex attributes
    in vec3 aPosition;
    in vec3 aNormal;
    in vec3 aColor;
    
    // Uniforms
    uniform mat4 uModelMatrix;
    uniform mat4 uViewMatrix;
    uniform mat4 uProjectionMatrix;
    uniform mat3 uNormalMatrix;
    
    // Output to fragment shader
    out vec3 vPosition;
    out vec3 vNormal;
    out vec3 vColor;
    
    void main() {
      // Transform position to world space
      vPosition = vec3(uModelMatrix * vec4(aPosition, 1.0));
      
      // Transform normal to world space
      vNormal = normalize(uNormalMatrix * aNormal);
      
      // Pass color to fragment shader
      vColor = aColor;
      
      // Final position in clip space
      gl_Position = uProjectionMatrix * uViewMatrix * vec4(vPosition, 1.0);
    }
  `,

  // Phong lighting model fragment shader
  phongFragment: `
    #version 300 es
    precision highp float;
    
    // Input from vertex shader
    in vec3 vPosition;
    in vec3 vNormal;
    in vec3 vColor;
    
    // Light uniforms
    uniform vec3 uLightPosition;
    uniform vec3 uLightColor;
    uniform vec3 uCameraPosition;
    uniform float uAmbient;
    uniform float uSpecular;
    
    // Output color
    out vec4 FragColor;
    
    void main() {
      // Normalize interpolated normal
      vec3 normal = normalize(vNormal);
      
      // Light direction
      vec3 lightDir = normalize(uLightPosition - vPosition);
      
      // View direction
      vec3 viewDir = normalize(uCameraPosition - vPosition);
      
      // Ambient component
      vec3 ambient = uAmbient * uLightColor;
      
      // Diffuse component
      float diff = max(dot(normal, lightDir), 0.0);
      vec3 diffuse = diff * uLightColor;
      
      // Specular component
      vec3 reflectDir = reflect(-lightDir, normal);
      float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
      vec3 specular = uSpecular * spec * uLightColor;
      
      // Combine all components
      vec3 result = (ambient + diffuse + specular) * vColor;
      
      FragColor = vec4(result, 1.0);
    }
  `,

  // Toon/Cel shading vertex shader
  toonVertex: `
    #version 300 es
    precision highp float;
    
    in vec3 aPosition;
    in vec3 aNormal;
    in vec3 aColor;
    
    uniform mat4 uModelMatrix;
    uniform mat4 uViewMatrix;
    uniform mat4 uProjectionMatrix;
    uniform mat3 uNormalMatrix;
    
    out vec3 vNormal;
    out vec3 vColor;
    out vec3 vPosition;
    
    void main() {
      vPosition = vec3(uModelMatrix * vec4(aPosition, 1.0));
      vNormal = normalize(uNormalMatrix * aNormal);
      vColor = aColor;
      gl_Position = uProjectionMatrix * uViewMatrix * vec4(vPosition, 1.0);
    }
  `,

  // Toon/Cel shading fragment shader
  toonFragment: `
    #version 300 es
    precision highp float;
    
    in vec3 vNormal;
    in vec3 vColor;
    in vec3 vPosition;
    
    uniform vec3 uLightPosition;
    uniform vec3 uLightColor;
    uniform vec3 uCameraPosition;
    
    out vec4 FragColor;
    
    void main() {
      vec3 normal = normalize(vNormal);
      vec3 lightDir = normalize(uLightPosition - vPosition);
      vec3 viewDir = normalize(uCameraPosition - vPosition);
      
      float diff = dot(normal, lightDir);
      
      // Quantize to create cel-shading effect
      if (diff > 0.7) {
        diff = 1.0;
      } else if (diff > 0.3) {
        diff = 0.6;
      } else if (diff > 0.0) {
        diff = 0.3;
      } else {
        diff = 0.0;
      }
      
      // Silhouette edge detection
      float silhouette = abs(dot(normal, viewDir));
      if (silhouette < 0.2) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0); // Black outline
      } else {
        vec3 result = diff * uLightColor * vColor;
        FragColor = vec4(result, 1.0);
      }
    }
  `,

  // Normal mapping vertex shader
  normalMapVertex: `
    #version 300 es
    precision highp float;
    
    in vec3 aPosition;
    in vec3 aNormal;
    in vec3 aTangent;
    in vec2 aTexCoord;
    in vec3 aColor;
    
    uniform mat4 uModelMatrix;
    uniform mat4 uViewMatrix;
    uniform mat4 uProjectionMatrix;
    uniform mat3 uNormalMatrix;
    
    out vec3 vTangent;
    out vec3 vBitangent;
    out vec3 vNormal;
    out vec2 vTexCoord;
    out vec3 vColor;
    out vec3 vPosition;
    
    void main() {
      vPosition = vec3(uModelMatrix * vec4(aPosition, 1.0));
      vNormal = normalize(uNormalMatrix * aNormal);
      vTangent = normalize(uNormalMatrix * aTangent);
      vBitangent = cross(vNormal, vTangent);
      vTexCoord = aTexCoord;
      vColor = aColor;
      
      gl_Position = uProjectionMatrix * uViewMatrix * vec4(vPosition, 1.0);
    }
  `,

  // Normal mapping fragment shader
  normalMapFragment: `
    #version 300 es
    precision highp float;
    
    in vec3 vTangent;
    in vec3 vBitangent;
    in vec3 vNormal;
    in vec2 vTexCoord;
    in vec3 vColor;
    in vec3 vPosition;
    
    uniform sampler2D uNormalMap;
    uniform vec3 uLightPosition;
    uniform vec3 uLightColor;
    uniform vec3 uCameraPosition;
    
    out vec4 FragColor;
    
    void main() {
      // Sample normal from normal map
      vec3 normalMapColor = texture(uNormalMap, vTexCoord).rgb;
      vec3 sampledNormal = normalize(normalMapColor * 2.0 - 1.0);
      
      // Create TBN matrix
      mat3 TBN = mat3(vTangent, vBitangent, vNormal);
      vec3 normal = normalize(TBN * sampledNormal);
      
      // Lighting calculations
      vec3 lightDir = normalize(uLightPosition - vPosition);
      vec3 viewDir = normalize(uCameraPosition - vPosition);
      vec3 reflectDir = reflect(-lightDir, normal);
      
      float diff = max(dot(normal, lightDir), 0.0);
      float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
      
      vec3 result = (0.2 + diff + spec * 0.5) * uLightColor * vColor;
      
      FragColor = vec4(result, 1.0);
    }
  `,

  // Parallax mapping vertex shader
  parallaxVertex: `
    #version 300 es
    precision highp float;
    
    in vec3 aPosition;
    in vec3 aNormal;
    in vec3 aTangent;
    in vec2 aTexCoord;
    in vec3 aColor;
    
    uniform mat4 uModelMatrix;
    uniform mat4 uViewMatrix;
    uniform mat4 uProjectionMatrix;
    uniform mat3 uNormalMatrix;
    
    out VS_OUT {
      vec3 FragPos;
      vec2 TexCoords;
      vec3 TangentViewPos;
      vec3 TangentFragPos;
    } vs_out;
    
    out vec3 vColor;
    
    void main() {
      vs_out.FragPos = vec3(uModelMatrix * vec4(aPosition, 1.0));
      vs_out.TexCoords = aTexCoord;
      
      vec3 T = normalize(mat3(uModelMatrix) * aTangent);
      vec3 N = normalize(mat3(uModelMatrix) * aNormal);
      T = normalize(T - dot(T, N) * N);
      vec3 B = cross(N, T);
      
      mat3 TBN = transpose(mat3(T, B, N));
      vs_out.TangentViewPos = TBN * vec3(0.0, 0.0, 0.0);
      vs_out.TangentFragPos = TBN * vs_out.FragPos;
      
      vColor = aColor;
      
      gl_Position = uProjectionMatrix * uViewMatrix * vec4(vs_out.FragPos, 1.0);
    }
  `,

  // Parallax mapping fragment shader
  parallaxFragment: `
    #version 300 es
    precision highp float;
    
    in VS_OUT {
      vec3 FragPos;
      vec2 TexCoords;
      vec3 TangentViewPos;
      vec3 TangentFragPos;
    } fs_in;
    
    in vec3 vColor;
    
    uniform sampler2D uTexture;
    uniform sampler2D uHeightMap;
    uniform float uHeightScale;
    
    out vec4 FragColor;
    
    vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir) {
      float height = texture(uHeightMap, texCoords).r;
      vec2 p = viewDir.xy / viewDir.z * (height * uHeightScale);
      return texCoords - p;
    }
    
    void main() {
      vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
      vec2 texCoords = ParallaxMapping(fs_in.TexCoords, viewDir);
      
      vec3 color = texture(uTexture, texCoords).rgb * vColor;
      FragColor = vec4(color, 1.0);
    }
  `,

  // Basic position-only vertex shader (for wireframe, outlines)
  basicVertex: `
    #version 300 es
    precision highp float;
    
    in vec3 aPosition;
    in vec3 aColor;
    
    uniform mat4 uModelMatrix;
    uniform mat4 uViewMatrix;
    uniform mat4 uProjectionMatrix;
    
    out vec3 vColor;
    
    void main() {
      vColor = aColor;
      gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPosition, 1.0);
    }
  `,

  // Basic fragment shader
  basicFragment: `
    #version 300 es
    precision highp float;
    
    in vec3 vColor;
    
    out vec4 FragColor;
    
    void main() {
      FragColor = vec4(vColor, 1.0);
    }
  `,

  // Depth visualization shader
  depthVertex: `
    #version 300 es
    precision highp float;
    
    in vec3 aPosition;
    
    uniform mat4 uModelMatrix;
    uniform mat4 uViewMatrix;
    uniform mat4 uProjectionMatrix;
    
    void main() {
      gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPosition, 1.0);
    }
  `,

  depthFragment: `
    #version 300 es
    precision highp float;
    
    out vec4 FragColor;
    
    void main() {
      float depth = gl_FragCoord.z;
      FragColor = vec4(vec3(depth), 1.0);
    }
  `,

  // Emission/Glow shader
  emissionVertex: `
    #version 300 es
    precision highp float;
    
    in vec3 aPosition;
    in vec3 aColor;
    
    uniform mat4 uModelMatrix;
    uniform mat4 uViewMatrix;
    uniform mat4 uProjectionMatrix;
    
    out vec3 vColor;
    
    void main() {
      vColor = aColor;
      gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPosition, 1.0);
    }
  `,

  emissionFragment: `
    #version 300 es
    precision highp float;
    
    in vec3 vColor;
    
    uniform float uEmissionIntensity;
    
    out vec4 FragColor;
    
    void main() {
      FragColor = vec4(vColor * uEmissionIntensity, 1.0);
    }
  `
};
