#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
using namespace std;

void extrude(shared_ptr<ALLEGRO_BITMAP> bmp, int pad)
{
    al_set_target_bitmap(bmp.get());
    int w = al_get_bitmap_width(bmp.get());
    int h = al_get_bitmap_height(bmp.get());
    int x = 0, y = 0;

    for (x = 0; x < w; x++) {
        auto color = al_get_pixel(bmp.get(), x, y + 1);
        al_put_pixel(x, y, color);
    }
    y = h - 1;
    for (x = 0; x < w; x++) {
        auto color = al_get_pixel(bmp.get(), x, y - 1);
        al_put_pixel(x, y, color);
    }
    x = 0;
    for (y = 0; y < h; y++) {
        auto color = al_get_pixel(bmp.get(), x + 1, y);
        al_put_pixel(x, y, color);
    }
    x = w - 1;
    for (y = 0; y < h; y++) {
        auto color = al_get_pixel(bmp.get(), x - 1, y);
        al_put_pixel(x, y, color);
    }
}

int main(int argc, char *argv[])
{
    al_init();
    al_init_image_addon();

    vector<string> args(argv + 1, argv + argc);

    if (args.size() < 2) {
        cout << "Usage: extrude <input> [<options>] <output>" << endl << endl << "Options:" << endl
             << "  size\t Tile size in pixels, default size=32" << endl
             << "  pad\t How much to extrude the tiles in pixels, default pad=1" << endl
             << "  space\t Empty space between tiles after extrusions in pixels, default space=0" << endl;
        return 0;
    }

    string input = args[0];
    string output = args[args.size() - 1];
    int size = 32;
    int pad = 1;
    int space = 0;

    string sizeopt("size=");
    string padopt("pad=");
    string spaceopt("space=");
    for (unsigned int i = 1; i < args.size() - 1; i++) {
        if (!args[i].compare(0, sizeopt.size(), sizeopt)) {
            size = atoi(args[i].substr(sizeopt.size()).c_str());
        }
        else if (!args[i].compare(0, padopt.size(), padopt)) {
            pad = atoi(args[i].substr(padopt.size()).c_str());
        }
        else if (!args[i].compare(0, spaceopt.size(), spaceopt)) {
            space = atoi(args[i].substr(spaceopt.size()).c_str());
        }
    }

    if (!al_filename_exists(input.c_str())) {
        cout << "Error: " << input << " doesn't exist!" << endl;
        return 0;
    }

    shared_ptr<ALLEGRO_BITMAP> src(al_load_bitmap(input.c_str()), al_destroy_bitmap);
    if (!src) {
        cout << "Error: " << input << " couldn't be loaded!" << endl;
        return 0;
    }

    int src_w = al_get_bitmap_width(src.get());
    int src_h = al_get_bitmap_height(src.get());

    if (src_w % size != 0 || src_h % size != 0) {
        cout << "Error: " << input << " not evenly divisible by " << size << endl;
        return 0;
    }

    int tiles_x = src_w / size;
    int tiles_y = src_h / size;
    int dst_w = tiles_x * (size + pad * 2 + space);
    int dst_h = tiles_y * (size + pad * 2 + space);

    shared_ptr<ALLEGRO_BITMAP> dst(al_create_bitmap(dst_w, dst_h), al_destroy_bitmap);
    al_set_target_bitmap(dst.get());
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));

    for (int i = 0; i < tiles_x; i++) {
        for (int j = 0; j < tiles_y; j++) {
            int x = i * size;
            int y = j * size;
            shared_ptr<ALLEGRO_BITMAP> tile(al_create_sub_bitmap(src.get(), x, y, size, size), al_destroy_bitmap);
            shared_ptr<ALLEGRO_BITMAP> extrudedTile(al_create_bitmap(size + pad * 2, size + pad * 2), al_destroy_bitmap);
            al_set_target_bitmap(extrudedTile.get());
            al_clear_to_color(al_map_rgba(0, 0, 0, 0));
            al_draw_bitmap(tile.get(), pad, pad, 0);
            extrude(extrudedTile, pad);

            int dx = i * (size + pad * 2 + space) + (space != 0 ? 1 : 0);
            int dy = j * (size + pad * 2 + space) + (space != 0 ? 1 : 0);
            al_set_target_bitmap(dst.get());
            al_draw_bitmap(extrudedTile.get(), dx, dy, 0);
        }
    }

    al_save_bitmap(output.c_str(), dst.get());

    return 0;
}
