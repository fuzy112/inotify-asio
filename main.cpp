#include <inotify-asio/inotify.hpp>
#include <iostream>

void watch_handler(inotify_asio::inotify &ino, boost::system::error_code ec, inotify_asio::event const &ev)
{
    if (ec)
    {
        std::cerr << "async watch: " << ec.message() << std::endl;
        return;
    }

    std::cout << "wd: " << ev.wd() << "\n"
                << "mask: " << ev.mask() << "\n"
                << "cookie: " << ev.cookie() << "\n"
                << "name: " << ev.name() << "\n";

    ino.async_watch([&ino](auto ec, auto ev)
    {
        watch_handler(ino, ec, ev);
    });
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " FILES...\n";
        return 1;
    }

    try {
        boost::asio::io_context ioc;

        inotify_asio::inotify ino(ioc);

        for (int i = 1; i < argc; ++i)
        {
            auto item = ino.add(argv[i], IN_CLOSE | IN_OPEN | IN_CREATE | IN_ACCESS | IN_MOVE | IN_DELETE);
            item.forget();
        }

        ino.async_watch([&ino](auto ec, auto ev)
        {
            watch_handler(ino, ec, ev);
        });

        ioc.run();
    }
    catch (std::exception const &exc)
    {
        std::cerr << exc.what() << std::endl;
    }
}
