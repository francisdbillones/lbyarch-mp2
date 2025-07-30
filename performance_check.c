#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

extern void compute_acceleration(size_t rows,
    const float* in,
    int* out);

/* reference implementation (must match asm semantics) */
static void accel_ref(size_t n, const float* in, int* out)
{
    const float k = 1000.0f / 3600.0f;      /* km/h → m/s */
    for (size_t i = 0; i < n; ++i) {
        float a = (in[i * 3 + 1] * k - in[i * 3] * k) / in[i * 3 + 2];
        out[i] = (int)lrintf(a);           /* round‑to‑nearest */
    }
}

/* elapsed microseconds between t0 and now */
static double qpc_us(LARGE_INTEGER t0, LARGE_INTEGER freq)
{
    LARGE_INTEGER t1;  QueryPerformanceCounter(&t1);
    return (t1.QuadPart - t0.QuadPart) * 1e6 / (double)freq.QuadPart;
}

int main(void)
{
    const size_t sizes[] = { 10, 100, 1'000, 10'000, 100'000 };
    const int    runs = 1'000;                /* per size */

    const size_t max_n = sizes[sizeof sizes / sizeof * sizes - 1];

    float* in = malloc(max_n * 3 * sizeof * in);
    int* out = malloc(max_n * sizeof * out);
    int* chk = malloc(max_n * sizeof * chk);
    if (!in || !out || !chk) {
        fputs("out‑of‑memory\n", stderr);
        return 1;
    }

    srand(42);                                  /* deterministic data */
    LARGE_INTEGER freq; QueryPerformanceFrequency(&freq);

    volatile long long sink = 0;                 /* stops dead‑code‑elim */

    for (size_t s = 0; s < sizeof sizes / sizeof * sizes; ++s) {
        size_t n = sizes[s];

        /* generate input once per size */
        for (size_t i = 0; i < n; ++i) {
            in[i * 3] = (float)(rand() % 201);      /* Vi 0‑200 km/h */
            in[i * 3 + 1] = (float)(rand() % 201);      /* Vf 0‑200 km/h */
            in[i * 3 + 2] = (float)(1 + rand() % 20);   /* T  1‑20 s     */
        }

        /* one‑off correctness check */
        accel_ref(n, in, chk);
        compute_acceleration(n, in, out);
        int ok = 1;
        for (size_t i = 0; i < n && ok; ++i)
            ok = (out[i] == chk[i]);
        printf("n=%zu  verify: %s\n", n, ok ? "ok" : "FAIL");

        /* timing: batch all runs inside one QPC window */
        LARGE_INTEGER t0;  QueryPerformanceCounter(&t0);
        for (int r = 0; r < runs; ++r) {
            compute_acceleration(n, in, out);
            for (size_t i = 0; i < n; ++i)      /* consume every result */
                sink += out[i];
        }
        double avg_us = qpc_us(t0, freq) / runs;
        printf("       avg %.3f microseconds over %d runs "
            "(~%.3f ns / elem)\n",
            avg_us, runs, 1e3 * avg_us / n);
    }

    /* keep sink from being optimised away entirely */
    printf("\nchecksum (ignore): %lld\n", sink);

    free(in); free(out); free(chk);
    return 0;
}
