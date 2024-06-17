/**
 * Triangle.c++ - Diyou.Engine
 *
 * Copyright (c) 2024 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
import Diyou;
import Shaders;

#include <array>
#include <cassert>
#include <iostream>

#include <webgpu/webgpu_cpp.h>

using namespace std;
using namespace wgpu;

constexpr char const *WindowTitle = "Hello Triangle";
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

  /* clang-format off */
  vector<float> const Vertices {
    -0.8F, -0.8F, 0.0F, 0.0F, 1.0F, // BL
    +0.8F, -0.8F, 0.0F, 1.0F, 0.0F, // BR
    -0.0F, +0.8F, 1.0F, 0.0F, 0.0F, // top
  };
  
  vector<uint16_t> const Indices {
    0,  1,  2, //
    0          //
  };
  /* clang-format on */

  float rotation = 0;
  Buffer vertexBuffer, indexBuffer, rotationBuffer;
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

    vertexBuffer = createBuffer(
      Vertices.data(),
      Vertices.size() * sizeof(decay_t<decltype(*Vertices.begin())>),
      BufferUsage::Vertex);

    indexBuffer = createBuffer(
      Indices.data(),
      Indices.size() * sizeof(decay_t<decltype(*Indices.begin())>),
      BufferUsage::Index);

    rotationBuffer =
      createBuffer(&rotation, sizeof(rotation), BufferUsage::Uniform);

    pipeline = CreatePipeline();

    Start();
  }

  void Draw() override
  {
    SurfaceTexture surfaceTexture;
    surface.GetCurrentTexture(&surfaceTexture);

    assert(surfaceTexture.texture);
    backBuffer = surfaceTexture.texture.CreateView();

    array colorAttachments{RenderPassColorAttachment{
      .view = backBuffer,
      .loadOp = LoadOp::Clear,
      .storeOp = StoreOp::Store,
      .clearValue = {.r = 0.3F, .g = 0.3F, .b = 0.3F, .a = 1.0F}}};

    RenderPassDescriptor renderPass{
      .colorAttachmentCount = colorAttachments.size(),
      .colorAttachments = colorAttachments.data()};

    CommandEncoder encoder = device.CreateCommandEncoder(nullptr);
    {
      RenderPassEncoder pass = encoder.BeginRenderPass(&renderPass);
      // update scene
      rotation += 0.1F;
      device.GetQueue().WriteBuffer(
        rotationBuffer, 0, &rotation, sizeof(rotation));
      // draw scene
      pass.SetPipeline(pipeline);
      pass.SetBindGroup(0, bindGroup, 0, 0);

      pass.SetVertexBuffer(0, vertexBuffer, 0, kWholeSize);
      pass.SetIndexBuffer(indexBuffer, IndexFormat::Uint16, 0, kWholeSize);
      pass.DrawIndexed(3, 1, 0, 0, 0);

      // finish
      pass.End();
    }

    CommandBuffer commands = encoder.Finish(nullptr);

    device.GetQueue().Submit(1, &commands);
  }

  RenderPipeline CreatePipeline()
  {
    ShaderModule shader = createWGSLShader(Shaders::WGSL::Triangle);

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
        .buffer = rotationBuffer,
        .offset = 0,
        .size = sizeof(rotation)}};
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
      VertexAttribute{
        .format = VertexFormat::Float32x2, .offset = 0, .shaderLocation = 0},
      VertexAttribute{
        .format = VertexFormat::Float32x3,
        .offset = 2 * sizeof(float),
        .shaderLocation = 1}};

    VertexBufferLayout vertexBufferLayout{
      .arrayStride = 5 * sizeof(float),
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
         .dstFactor = BlendFactor::One},
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
      RenderPipelineDescriptor descriptor{
        .layout = pipelineLayout,
        .vertex = vertexState,
        .primitive =
          {.topology = PrimitiveTopology::TriangleList,
           .stripIndexFormat = IndexFormat::Undefined,
           .frontFace = FrontFace::CCW,
           .cullMode = CullMode::None},
        .depthStencil = nullptr,
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
  for (int i = 0; i < 2; i++) {
    auto window = make_unique<Window>(
      format("{}#{}", WindowTitle, i), WindowWidth, WindowHeight);
    Renderer::CreateFrom(::move(window));
  }
}
