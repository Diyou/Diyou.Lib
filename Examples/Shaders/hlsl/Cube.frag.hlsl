struct FragmentInput {
    float2 fragUV : TEXCOORD0;
    float4 fragPosition : TEXCOORD1;
};

float4 main(FragmentInput input) : SV_TARGET {
    return input.fragPosition;
}
