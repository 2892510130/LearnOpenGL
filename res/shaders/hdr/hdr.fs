#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform int hdr;
uniform bool enable_gamma;
uniform float exposure;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;

    if(hdr == 1)
    {
        // exposure
        hdrColor = vec3(1.0) - exp(-hdrColor * exposure);
    }
    else if (hdr == 2)
    {
        // reinhard
        hdrColor = hdrColor / (hdrColor + vec3(1.0));
    }

    if (enable_gamma)
        hdrColor = pow(hdrColor, vec3(1.0 / gamma));
    
    FragColor = vec4(hdrColor, 1.0);
}
