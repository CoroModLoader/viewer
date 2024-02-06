#include <nfd.hpp>
#include <fmt/format.h>

#include <embedded/all.hpp>
#include <saucer/smartview.hpp>
#include <saucer/utils/future.hpp>

#include "config.hpp"
#include "constants.hpp"
#include "controller.hpp"

int main()
{
    saucer::smartview saucer;

    saucer.set_title(fmt::format("CoroViewer | v{}", viewer::version));
    saucer.set_min_size(900, 600);

    viewer::controller ctrl{};

    saucer.expose("decompile", [&ctrl](const std::string &name) { return ctrl.decompile(name); });
    saucer.expose("list", [&ctrl]() { return ctrl.list(); });

    saucer.expose("load",
                  [&ctrl](const std::string &path) -> std::variant<bool, std::string>
                  {
                      auto rtn = ctrl.load(path);

                      if (!rtn.has_value())
                      {
                          return rtn.error();
                      }

                      return true;
                  });

    std::atomic_size_t counter{0};
    std::optional<std::stop_source> token;
    std::optional<std::jthread> watcher_thread;

    auto watcher = [&saucer, &counter, &ctrl](auto token)
    {
        const auto total = ctrl.list().size();

        while (!token.stop_requested())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(750));

            const auto current = counter.load();

            if (current >= total)
            {
                break;
            }

            saucer.evaluate<void>("window.exportProgress({}, {})", current, total) | saucer::forget();
        }

        saucer.evaluate<void>("window.exportProgress({}, {})", -1, total) | saucer::forget();
    };

    saucer.expose("export",
                  [&ctrl, &counter, &token, &watcher, &watcher_thread](const std::string &path)
                  {
                      if (token)
                      {
                          token->request_stop();
                      }

                      counter  = 0;
                      auto rtn = ctrl.export_archive(path, counter);

                      if (!rtn.has_value())
                      {
                          return false;
                      }

                      token.emplace(std::move(rtn.value()));
                      watcher_thread.emplace(watcher, token.value());

                      return true;
                  });

    saucer.expose("cancel",
                  [&token]()
                  {
                      if (!token)
                      {
                          return;
                      }

                      token->request_stop();
                      token.reset();
                  });

    static auto &config = viewer::config::load();

    saucer.expose("unluac-path", [] { return config.unluac_path; });
    saucer.expose("java-path", [] { return config.java_path; });

    saucer.expose("set-unluac-path",
                  [](std::optional<std::string> path)
                  {
                      config.unluac_path = std::move(path);
                      config.save();
                  });

    saucer.expose("set-java-path",
                  [](std::optional<std::string> path)
                  {
                      config.java_path = std::move(path);
                      config.save();
                  });

    NFD_Init();

    saucer.expose("choose-path",
                  [](std::optional<std::string> origin) -> std::optional<std::string>
                  {
                      NFD::UniquePath path;
                      auto result = NFD::OpenDialog(path, nullptr, 0, origin.has_value() ? origin->c_str() : nullptr);

                      if (result != NFD_OKAY)
                      {
                          return std::nullopt;
                      }

                      return path.get();
                  });

    saucer.expose("choose-folder",
                  [](std::optional<std::string> origin) -> std::optional<std::string>
                  {
                      NFD::UniquePath path;
                      auto rtn = NFD::PickFolder(path, origin.has_value() ? origin->c_str() : nullptr);

                      if (rtn != NFD_OKAY)
                      {
                          return std::nullopt;
                      }

                      return path.get();
                  });

    saucer.set_context_menu(false);

    saucer.embed(saucer::embedded::all());
    saucer.serve("index.html");

    saucer.show();
    saucer.run();

    if (token)
    {
        token->request_stop();
    }

    return 0;
}
