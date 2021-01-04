#include "HEPT.h"

uint32_t hept32_state = 7;

INLINE uint32_t hept32() {
    hept32_state *= 0x24B798CCB40768FEu;
    return hept32_state >> 16;
}

/*INLINE uint32_t hept32_seed(uint64_t seed) {
    seed += 0x6EBD09CCA107EBDDu;
    return (seed + (((seed * 0x44C1FC5027002A57u)) | (((seed + 0x1415F0B7E9A9A481u) * 0x59DD666EE404A405u) )) * 0x87F39CD3B7BCDE8Bu) >> 7;
}*/

static u64 _HEPT32_SEED = 7;
INLINE void hept64_seed(uint64_t seed) {
    _HEPT32_SEED = ((seed + 0x657AE927E64796D9u) * 0x82DF2834F6CA22F5u);
}

INLINE u32 hept32(uint64_t x, uint64_t y) {
    x = (x * 0x327E04108870786Du) + _HEPT32_SEED;
    y = (y * 0x6D10E6211F72C1A3u) + _HEPT32_SEED;
    return ((((((x + 0x6EBD09CCA107EBDDu) * 0x44C1FC5027002A57u) | ((x + 0x1415F0B7E9A9A481u) * 0x59DD666EE404A405u)) * 0x87F39CD3B7BCDE8Bu) >> 7) *
        (((((y + 0x28F3871E750F59C5u) * 0x4EA6638342AD8D9Fu) | ((y + 0x3B3A30577BE038B7u) * 0x320119263D56642Bu)) * 0x7338441161290A3Du) >> 7)) >> 7;
}

INLINE u32 bitmul(s32 a, s32 b) {
    return 1;
}

INLINE u32 bitlerp(s32 a, s32 b, s32 n) {
    return (a + ((b - a) * n / 255));
}

INLINE u32 smoothstep(u32 t) {
    return (bitlerp(0, bitlerp(0, t, t), (0x2fd - bitlerp(0, t, 0x200))));
}

START
SYSTEM_INIT

u32 noise_w = 2048, noise_h = 2048;

window_set_size(1024, 1024);

SDL_Texture* noise = SDL_CreateTexture(RENDERER, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, noise_w, noise_h);
list_u32 pixels;

list_u32 l1;
list_u32 l2;

for (u16 y = 0; y < noise_h; y++)
    for (u16 x = 0; x < noise_w; x++)
    {
        pixels.push_back(0);
    }

MAINLOOP_START

static s64 N = 0, M = 0, cc = 0;

//if (KEYBOARD_KEY_PRESSED[SDL_SCANCODE_RIGHT]) N += noise_w;
//if (KEYBOARD_KEY_PRESSED[SDL_SCANCODE_DOWN]) M += noise_w;
N += (KEYCHECK_RIGHT - KEYCHECK_LEFT);
M += (KEYCHECK_DOWN - KEYCHECK_UP);

hept64_seed(7);

u64 _hept = 0;
u8 gx = 0, gy = 0;
u32 tx = 0, ty = 0;

static u32 divn = 16;
static u32 addn = 256 / divn;
u64 _time = 0;
TIMER_START
for (u32 y = 0; y < noise_h; y++)
{
    gx = 0;
    for (u32 x = 0; x < noise_w; x++)
    {
        //heptrand_seed((y + M) * noise_w + (x + N));
        //pixels[y * noise_w + x] = heptrand();
        //pixels[y * noise_w + x] = (uint32_t)(vnoise(vec3<f32>{ (f32)(y + M) * 0.1f, (f32)(x + N) * 0.1f,1.0f }) * (float)0xffffffff);// (uint32_t)hept32_seed((y + M) * noise_w + (x + N));
        //_hept = hept64((u64)x + (u64)N, (u64)y + (u64)M);
        //pixels[y * noise_w + x] = (uint32_t)(_hept & 0x00000000ffffffff);
        //pixels[y * noise_w + (x+1)] = (uint32_t)((_hept & 0xffffffff00000000) >> 32);
        /*_hept = ((((u64)(hept32(x, y) & 0x000000ff)) >> 1) +
            (((u64)(hept32(x - 1, y) & 0x000000ff) +
                (u64)(hept32(x + 1, y) & 0x000000ff) +
                (u64)(hept32(x, y - 1) & 0x000000ff) +
                (u64)(hept32(x, y + 1) & 0x000000ff)) >> 3)) & 0x000000ff;*/
        tx = (x + N) / divn;
        ty = (y + M) / divn;
        u32 c00 = (hept32(tx, ty) & 0x000000ff);
        u32 c10 = (hept32(tx + 1, ty) & 0x000000ff);
        u32 c01 = (hept32(tx, ty + 1) & 0x000000ff);
        u32 c11 = (hept32(tx + 1, ty + 1) & 0x000000ff);

        s32 sx = smoothstep((s32)gx);
        _hept = bitlerp(bitlerp(c00, c10, sx), bitlerp(c01, c11, sx), smoothstep((s32)gy));

        _hept = (_hept > 127) ? 0xffff : 0;

        pixels[y * noise_w + x] = (uint32_t)(((_hept) | (_hept << 8) | (_hept << 16)) | 0xff000000);
        gx += addn;
    }
    gy += addn;
}
_time = TIMER_END;
PRINT(_time);

PRINT(" ");

while (true)
{

}

SDL_UpdateTexture(noise, nullptr, &pixels[0], sizeof(uint32_t) * noise_w);

SDL_RenderCopy(RENDERER, noise, nullptr, nullptr);

MAINLOOP_END
END





/*
auto _ps = _pixels.size();
std::stack<u32> _delstack;
u32 _deln = 0;
for (u32 p = 0; p < _ps - 1; p++)
{
    if (p > _pixels.size() - 1) break;
    _pixels[p].i = _pixels[p].i & 0x00ff;
    if (_pixels[p + 1].i != _pixels[p].i) break;

    for (u16 x = 1; x < 16; x++)
    {
        if (p + x > _pixels.size() - 1) break;
        if (_pixels[p + x].i != _pixels[p].i) break;
        if (_pixels[p + x].x == _pixels[p].x + x) {
            _pixels[p].i += 0x1000;
            _delstack.push((p + 1) - _deln);
            _deln++;
            p++;
        }
    }

    for (u16 y = 1; y < 16; y++)
    {
        if (p + y > _pixels.size() - 1) break;
        if (_pixels[p + y].i != _pixels[p].i) break;
        if (_pixels[p + y].x == _pixels[p].x + y) {
            _pixels[p].i += 0x0100;
            _delstack.push((p + _csurf->w) - _deln);
            _deln++;
            p++;
        }
    }

    /*if (_pixels[p + 1].x == _pixels[p].x + 1) {
        _pixels[p].i = ((_pixels[p].i) & 0x00ff) | (((_pixels[p + 1].i) << 8) & 0xff00);
        _delstack.push((p + 1) - _deln);
        _deln++;
        p++;
    }* /
}
/ *
auto _ps = _pixels.size();
std::stack<u32> _delstack;
u32 _deln = 0;
for (u32 p = 0; p < _ps - 1; p++)
{
    if (p > _pixels.size() - 1) break;
    _pixels[p].i = _pixels[p].i & 0x0001;
    for (u32 i = 1; i < 8; i++)
    {
        if (p + i > _pixels.size() - 1) break;

        if (_pixels[p + i].x == _pixels[p].x + i) {
            _pixels[p].i = _pixels[p].i | ((_pixels[p + i].i & 0x0001) << i);
            _delstack.push((p + i) - _deln);
            _deln++;
            p++;
        }
    }
}
* /

while (!_delstack.empty()) {
    _pixels.erase(_pixels.begin() + _delstack.top());
    _delstack.pop();
}
*/