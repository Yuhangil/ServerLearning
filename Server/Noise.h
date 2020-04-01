#pragma once

#include "Server.h"

class CNoise
{
private:
    /// Initial permutation table
    static const int Source[256];

    const int RandomSize = 256;
    const double Sqrt3 = 1.7320508075688772935;
    const double Sqrt5 = 2.2360679774997896964;
    int* _random;

    /// Skewing and unskewing factors for 2D, 3D and 4D, 
    /// some of them pre-multiplied.
    const double F2 = 0.5 * (Sqrt3 - 1.0);

    const double G2 = (3.0 - Sqrt3) / 6.0;
    const double G22 = G2 * 2.0 - 1;

    const double F3 = 1.0 / 3.0;
    const double G3 = 1.0 / 6.0;

    const double F4 = (Sqrt5 - 1.0) / 4.0;
    const double G4 = (5.0 - Sqrt5) / 20.0;
    const double G42 = G4 * 2.0;
    const double G43 = G4 * 3.0;
    const double G44 = G4 * 4.0 - 1.0;

    /// <summary>
    /// Gradient vectors for 3D (pointing to mid points of all edges of a unit
    /// cube)
    /// </summary>
    static const int Grad3[][3];

public :
    CNoise();
    CNoise(int seed);
    ~CNoise();

public:
    float Evaluate(VECTOR vector);

private:
    void Randomize(int seed);

private:
    static double Dot(const int g[], double x, double y, double z, double t)
    {
        return g[0] * x + g[1] * y + g[2] * z + g[3] * t;
    }

    static double Dot(const int g[], double x, double y, double z)
    {
        return g[0] * x + g[1] * y + g[2] * z;
    }

    static double Dot(const int g[], double x, double y)
    {
        return g[0] * x + g[1] * y;
    }

    static int FastFloor(double x)
    {
        return x >= 0 ? (int)x : (int)x - 1;
    }

    static void UnpackLittleUint32(int value, byte* buffer)
    {
        buffer[0] = (byte)(value & 0x00ff);
        buffer[1] = (byte)((value & 0xff00) >> 8);
        buffer[2] = (byte)((value & 0x00ff0000) >> 16);
        buffer[3] = (byte)((value & 0xff000000) >> 24);
    }
};

