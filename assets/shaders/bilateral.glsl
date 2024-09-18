#type vertex
#version 450

layout(location = 0) in vec3 a_PositionOS;

out vec2 v_TexCoord;

void main()
{
	gl_Position = vec4(a_PositionOS, 1.0);
    v_TexCoord = (a_PositionOS.xy + 1.0) / 2.0;
}

#type fragment
#version 450

layout(location = 0) out vec4 FinalColor;

in vec2 v_TexCoord;

layout(binding = 0) uniform sampler2D u_Texture;

uniform float u_SpatialSigma = 2.0;   // Spatial standard deviation
uniform float u_ColorSigma = 1.0;     // Color standard deviation
uniform int u_FilterRadius = 5;       // Filter radius

void main()
{
    vec3 centerColor = texture(u_Texture, v_TexCoord).rgb;
    float spatialWeightSum = 0.0;
    vec3 colorWeightedSum = vec3(0.0);

    for (int x = -u_FilterRadius; x <= u_FilterRadius; ++x)
    {
        for (int y = -u_FilterRadius; y <= u_FilterRadius; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) / textureSize(u_Texture, 0);
            vec3 sampleColor = texture(u_Texture, v_TexCoord + offset).rgb;

            // Calculate spatial weight
            float spatialDistance = sqrt(float(x * x + y * y));
            float spatialWeight = exp(-(spatialDistance * spatialDistance) / (2.0 * u_SpatialSigma * u_SpatialSigma));

            // Calculate color weight
            float colorDistance = length(sampleColor - centerColor);
            float colorWeight = exp(-(colorDistance * colorDistance) / (2.0 * u_ColorSigma * u_ColorSigma));

            // Combined weight
            float combinedWeight = spatialWeight * colorWeight;

            // Accumulate weighted color and weights
            colorWeightedSum += sampleColor * combinedWeight;
            spatialWeightSum += combinedWeight;
        }
    }

    // Normalize the result
    FinalColor = vec4(colorWeightedSum / spatialWeightSum, 1.0);
}
