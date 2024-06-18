import Diyou;

#include <webgpu/webgpu_cpp.h>

using namespace std;
using namespace wgpu;

constexpr char const *WindowTitle = "WebGPU Window";
constexpr unsigned WindowWidth = 720;
constexpr unsigned WindowHeight = 480;

struct Renderer
: Runtime<Renderer>
, public virtual Window
, public virtual Context
// Interfaces:
, public virtual IUtilities
{
  Renderer(unique_ptr<Window> &window, unique_ptr<Context> &context)
  : Window(::move(*window))
  , Context(::move(*context))
  {
    // TODO Yourney begins here
  }

  void Close() override { Runtime::Close(); }

  static void CreateFrom(
    unique_ptr<Window> &window,
    unique_ptr<Context> &context)
  {
    auto instance = make_unique<Renderer>(window, context);
    Runtime::Add(instance);
  }
};

void
Init(Application const &app)
{
  auto window = make_unique<Window>(WindowTitle, WindowWidth, WindowHeight);
  Context::Receive(window, Renderer::CreateFrom);
}
