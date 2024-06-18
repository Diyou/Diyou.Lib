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
  Renderer(Window &&window, Context &&context)
  : Window(::move(window))
  , Context(::move(context))
  {
    // TODO Yourney begins here
  }

  void Close() override { Runtime::Close(); }

  static void CreateFrom(unique_ptr<Window> &window)
  {
    Context::From(
      window,
      [](Window &&window, Context &&context)
      {
        auto renderer = make_unique<Renderer>(::move(window), ::move(context));
        Instances.push_back(::move(renderer));
      });
  }
};

void
Init(Application const &app)
{
  auto window = make_unique<Window>(WindowTitle, WindowWidth, WindowHeight);
  Renderer::CreateFrom(window);
}
