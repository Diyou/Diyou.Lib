/**
 * Cube.c++ - Diyou.Engine
 *
 * Copyright (c) 2024 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
import Diyou;
import Shaders;

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

#include <array>
#include <cassert>
#include <chrono>
#include <iostream>

#include <webgpu/webgpu_cpp.h>

using namespace std;
using namespace chrono;
using namespace wgpu;
using namespace glm;

constexpr char const *WindowTitle = "Hello Cube";
constexpr unsigned WindowWidth = 720;
constexpr unsigned WindowHeight = 480;

struct Renderer
: public virtual Window
, public virtual Context
// Interfaces:
, public virtual ILoop
, public virtual IUtilities
{
  TextureView backBuffer;

  TextureFormat textureFormat;
  Texture depthTexture;

  /* clang-format off */
  vector<float> const Vertices = {
    // float4 position, float4 color, float2 uv,
    1,  -1, 1,  1, 1, 0, 1, 1, 0, 1, //
    -1, -1, 1,  1, 0, 0, 1, 1, 1, 1, //
    -1, -1, -1, 1, 0, 0, 0, 1, 1, 0, //
    1,  -1, -1, 1, 1, 0, 0, 1, 0, 0, //
    1,  -1, 1,  1, 1, 0, 1, 1, 0, 1, //
    -1, -1, -1, 1, 0, 0, 0, 1, 1, 0, //

    1,  1,  1,  1, 1, 1, 1, 1, 0, 1, //
    1,  -1, 1,  1, 1, 0, 1, 1, 1, 1, //
    1,  -1, -1, 1, 1, 0, 0, 1, 1, 0, //
    1,  1,  -1, 1, 1, 1, 0, 1, 0, 0, //
    1,  1,  1,  1, 1, 1, 1, 1, 0, 1, //
    1,  -1, -1, 1, 1, 0, 0, 1, 1, 0, //

    -1, 1,  1,  1, 0, 1, 1, 1, 0, 1, //
    1,  1,  1,  1, 1, 1, 1, 1, 1, 1, //
    1,  1,  -1, 1, 1, 1, 0, 1, 1, 0, //
    -1, 1,  -1, 1, 0, 1, 0, 1, 0, 0, //
    -1, 1,  1,  1, 0, 1, 1, 1, 0, 1, //
    1,  1,  -1, 1, 1, 1, 0, 1, 1, 0, //

    -1, -1, 1,  1, 0, 0, 1, 1, 0, 1, //
    -1, 1,  1,  1, 0, 1, 1, 1, 1, 1, //
    -1, 1,  -1, 1, 0, 1, 0, 1, 1, 0, //
    -1, -1, -1, 1, 0, 0, 0, 1, 0, 0, //
    -1, -1, 1,  1, 0, 0, 1, 1, 0, 1, //
    -1, 1,  -1, 1, 0, 1, 0, 1, 1, 0, //

    1,  1,  1,  1, 1, 1, 1, 1, 0, 1, //
    -1, 1,  1,  1, 0, 1, 1, 1, 1, 1, //
    -1, -1, 1,  1, 0, 0, 1, 1, 1, 0, //
    -1, -1, 1,  1, 0, 0, 1, 1, 1, 0, //
    1,  -1, 1,  1, 1, 0, 1, 1, 0, 0, //
    1,  1,  1,  1, 1, 1, 1, 1, 0, 1, //

    1,  -1, -1, 1, 1, 0, 0, 1, 0, 1, //
    -1, -1, -1, 1, 0, 0, 0, 1, 1, 1, //
    -1, 1,  -1, 1, 0, 1, 0, 1, 1, 0, //
    1,  1,  -1, 1, 1, 1, 0, 1, 0, 0, //
    1,  -1, -1, 1, 1, 0, 0, 1, 0, 1, //
    -1, 1,  -1, 1, 0, 1, 0, 1, 1, 0, //
  };
  /* clang-format on */

  mat4 transformation;
  Buffer vertexBuffer, transformationBuffer;
  BindGroup bindGroup;
  RenderPipeline pipeline;

  Renderer(Window &&window, Context &&context)
  : Window(::move(window))
  , Context(::move(context))
  {
    unsigned width;
    unsigned height;
    GetSize(width, height);

    auto properties = getAdapterProperties();

    if (properties.name != nullptr && properties.driverDescription != nullptr) {
      cout << format(
        "GPU: {}\nDriver: {}\n", properties.name, properties.driverDescription);
    }

    SurfaceCapabilities capabilities;
    surface.GetCapabilities(adapter, &capabilities);

    vector<PresentMode> presentModes(
      capabilities.presentModes,
      capabilities.presentModes + capabilities.presentModeCount);

    vector<CompositeAlphaMode> alphaModes(
      capabilities.alphaModes,
      capabilities.alphaModes + capabilities.alphaModeCount);

    textureFormat = capabilities.formats[0];

    SurfaceConfiguration config{
      .device = device,
      .format = textureFormat,
      .usage = TextureUsage::RenderAttachment,
      .alphaMode = alphaModes.front(),
      .width = width,
      .height = height,
      .presentMode = presentModes.front()};

    surface.Configure(&config);

    {
      TextureDescriptor descriptor{
        .usage = TextureUsage::RenderAttachment,
        .size = {.width = width, .height = height},
        .format = TextureFormat::Depth24Plus,
      };
      depthTexture = device.CreateTexture(&descriptor);
    }

    vertexBuffer = createBuffer(
      Vertices.data(),
      Vertices.size() * sizeof(decay_t<decltype(*Vertices.begin())>),
      BufferUsage::Vertex);

    transformationBuffer = createBuffer(
      &transformation, sizeof(transformation), BufferUsage::Uniform);

    pipeline = CreatePipeline();

    Start();
  }

  mat4 CreateCamera(float distance)
  {
    float aspectRatio = 720.0F / 480.0F;
    float fovDeg = 60;
    mat4 projection =
      perspective(2 * pi<float>() * (fovDeg / 360), aspectRatio, 0.1F, 100.0F);

    mat4 translation = translate(mat4(1.0F), vec3(0.0F, 0.0F, -distance));

    auto millis =
      duration_cast<milliseconds>(system_clock::now().time_since_epoch())
        .count();

    double now = duration<double>(millis).count();
    now /= 1000;

    vec3 rotation(sin(now), cos(now), 0);
    translation = rotate(translation, rotation.y, vec3(1.0F, 0.0F, 0.0F));
    translation = rotate(translation, rotation.x, vec3(0.0F, 1.0F, 0.0F));
    translation = rotate(translation, rotation.z, vec3(0.0F, 0.0F, 1.0F));

    mat4 scaling = scale(mat4(1.0F), vec3(1.0F));

    return projection * translation * scaling;
  };

  void Draw() override
  {
    SurfaceTexture surfaceTexture;
    surface.GetCurrentTexture(&surfaceTexture);
    backBuffer = surfaceTexture.texture.CreateView();

    array colorAttachments{RenderPassColorAttachment{
      .view = backBuffer,
      .loadOp = LoadOp::Clear,
      .storeOp = StoreOp::Store,
      .clearValue = {.r = 0.5F, .g = 0.5F, .b = 0.5F, .a = 1.0F}}};

    RenderPassDepthStencilAttachment depthStencil{
      .view = depthTexture.CreateView(),
      .depthLoadOp = LoadOp::Clear,
      .depthStoreOp = StoreOp::Store,
      .depthClearValue = 1.0F};
    RenderPassDescriptor renderPass{
      .colorAttachmentCount = colorAttachments.size(),
      .colorAttachments = colorAttachments.data(),
      .depthStencilAttachment = &depthStencil};

    CommandEncoder encoder = device.CreateCommandEncoder(nullptr);
    {
      RenderPassEncoder pass = encoder.BeginRenderPass(&renderPass);
      // update scene

      transformation = CreateCamera(4);

      device.GetQueue().WriteBuffer(
        transformationBuffer, 0, &transformation, sizeof(transformation));
      // draw scene
      pass.SetPipeline(pipeline);
      pass.SetBindGroup(0, bindGroup);

      pass.SetVertexBuffer(0, vertexBuffer);
      pass.Draw(Vertices.size() / 10);

      // finish
      pass.End();
    }

    CommandBuffer commands = encoder.Finish(nullptr);

    device.GetQueue().Submit(1, &commands);
  }

  RenderPipeline CreatePipeline()
  {
    ShaderModule shader = createWGSLShader(Shaders::WGSL::Cube);

    BindGroupLayout bindGroupLayout;
    {
      array entries{BindGroupLayoutEntry{
        .binding = 0,
        .visibility = wgpu::ShaderStage::Vertex,
        .buffer = {.type = BufferBindingType::Uniform}}};

      BindGroupLayoutDescriptor descriptor{
        .entryCount = entries.size(), .entries = entries.data()};

      bindGroupLayout = device.CreateBindGroupLayout(&descriptor);
    }

    // BindGroup
    {
      array entries{BindGroupEntry{
        .binding = 0,
        .buffer = transformationBuffer,
        .offset = 0,
        .size = sizeof(transformation)}};

      BindGroupDescriptor descriptor{
        .layout = bindGroupLayout,
        .entryCount = entries.size(),
        .entries = entries.data()};
      bindGroup = device.CreateBindGroup(&descriptor);
    }

    // TODO Support multiple layouts
    PipelineLayout pipelineLayout;
    {
      PipelineLayoutDescriptor descriptor{
        .bindGroupLayoutCount = 1, .bindGroupLayouts = &bindGroupLayout};
      pipelineLayout = device.CreatePipelineLayout(&descriptor);
    }

    array vertexAttributes{
      // Position
      VertexAttribute{
        .format = VertexFormat::Float32x4, .offset = 0, .shaderLocation = 0},
      // UV
      VertexAttribute{
        .format = VertexFormat::Float32x2,
        .offset = 8 * sizeof(float),
        .shaderLocation = 1}};

    VertexBufferLayout vertexBufferLayout{
      .arrayStride = 10 * sizeof(float),
      .attributeCount = vertexAttributes.size(),
      .attributes = vertexAttributes.data()};

    VertexState vertexState{
      .module = shader,
      .entryPoint = "main",
      .bufferCount = 1,
      .buffers = &vertexBufferLayout};

    BlendState blendState{
      .color =
        {.operation = BlendOperation::Add,
         .srcFactor = BlendFactor::One,
         .dstFactor = BlendFactor::Zero},
      .alpha = {
        .operation = BlendOperation::Add,
        .srcFactor = BlendFactor::One,
        .dstFactor = BlendFactor::One}};

    array colorTargets{ColorTargetState{
      .format = textureFormat,
      .blend = &blendState,
      .writeMask = ColorWriteMask::All}};

    FragmentState fragmentState{
      .module = shader,
      .entryPoint = "frag_main",
      .targetCount = colorTargets.size(),
      .targets = colorTargets.data()};

    // RenderPipeline
    {
      DepthStencilState depthStencil{
        .format = TextureFormat::Depth24Plus,
        .depthWriteEnabled = true,
        .depthCompare = CompareFunction::Less};
      RenderPipelineDescriptor descriptor{
        .layout = pipelineLayout,
        .vertex = vertexState,
        .primitive =
          {.topology = PrimitiveTopology::TriangleList,
           .stripIndexFormat = IndexFormat::Undefined,
           .frontFace = FrontFace::CCW,
           .cullMode = CullMode::Back},
        .depthStencil = &depthStencil,
        .multisample =
          {.count = 1, .mask = 0xFFFFFFFF, .alphaToCoverageEnabled = false},
        .fragment = &fragmentState};

      return device.CreateRenderPipeline(&descriptor);
    }
  }

  void Close() override
  {
    Stop();

    auto position = find_if(
      instances.begin(),
      instances.end(),
      [this](auto &renderer) { return renderer.get() == this; });

    assert(position != instances.end());

    instances.erase(position);
  }

  static void CreateFrom(unique_ptr<Window> window)
  {
    Context::From(
      ::move(window),
      [](Window &&window, Context &&context)
      {
        auto renderer = make_unique<Renderer>(::move(window), ::move(context));
        instances.push_back(::move(renderer));
      });
  }

  static vector<unique_ptr<Renderer>> instances;
};

vector<unique_ptr<Renderer>> Renderer::instances;

void
Init(Runtime const &runtime)
{
  auto window = make_unique<Window>(WindowTitle, WindowWidth, WindowHeight);
  Renderer::CreateFrom(::move(window));
}
