cbuffer Uniforms {
    float4x4 modelViewProjectionMatrix;
};

struct VertexInput {
    float4 position : POSITION;
    float2 uv : TEXCOORD0;
};

struct VertexOutput {
    float4 position : SV_POSITION;
    float2 fragUV : TEXCOORD0;
    float4 fragPosition : TEXCOORD1;
};

VertexOutput main(VertexInput input) {
    VertexOutput output;
    output.position = mul(modelViewProjectionMatrix, input.position);
    output.fragUV = input.uv;
    output.fragPosition = 0.5 * (input.position + float4(1.0, 1.0, 1.0, 1.0));
    return output;
}
