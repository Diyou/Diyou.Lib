import Diyou;

#include <cassert>

#include <webgpu/webgpu_cpp.h>

using namespace std;
using namespace wgpu;

constexpr char const *WindowTitle = "WebGPU Window";
constexpr unsigned WindowWidth = 720;
constexpr unsigned WindowHeight = 480;

struct Renderer
: public virtual Window
, public virtual Context
// Interfaces:
, public virtual IRenderer
{
  Renderer(Window &&window, Context &&context)
  : Window(::move(window))
  , Context(::move(context))
  {
    // TODO Yourney begins here
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
  static void CreateFrom(unique_ptr<Window> &window)
  {
    Context::From(
      window,
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
  Renderer::CreateFrom(window);
}
