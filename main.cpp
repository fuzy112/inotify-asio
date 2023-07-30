#include <inotify-asio/inotify.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/detached.hpp>
#include <span>
#include <iostream>

using boost::asio::use_awaitable;
using boost::asio::use_awaitable_t;
using inotify = use_awaitable_t<>::as_default_on_t<inotify_asio::inotify>;

boost::asio::awaitable<void> 
watch(boost::asio::io_context &ioc, std::span<const char *> files)
{
    try
    {
        inotify ino(ioc);

        for (auto file : files)
        {
            auto item = ino.add(file,IN_CLOSE | IN_OPEN | IN_CREATE | IN_ACCESS | IN_MOVE | IN_DELETE);
            item.forget();
        }

        for (;;)
        {
            auto ev = co_await ino.async_watch();
            std::cout << "wd: " << ev.wd() << "\n"
                        << "mask: " << ev.mask() << "\n"
                        << "cookie: " << ev.cookie() << "\n"
                        << "name: " << ev.name() << "\n";
        }
    }
    catch (std::exception &exc)
    {
        std::cerr << exc.what() << std::endl;
    }
}

int main(int argc, const char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " FILES...\n";
        return 1;
    }

    try {
        boost::asio::io_context ioc;
        boost::asio::co_spawn(ioc, watch(ioc, std::span<const char *>(argv + 1, argc - 1)), boost::asio::detached);
        ioc.run();
    }
    catch (std::exception const &exc)
    {
        std::cerr << exc.what() << std::endl;
    }
}
