#include <chrono>

#include "mat.h"
#include "program.h"
#include "uniforms.h"
#include "wavefront.h"
#include "texture.h"

#include "orbiter.h"
#include "draw.h"

#include "app.h"        // classe Application a deriver
#include "app_time.h"        // classe Application a deriver
#include "text.h"

#include <array>
#include "ScalarField.h"
#include "Heightfield.h"

#include "PoissonDiscSampling.h"

class Util
{
public:
    Util() = delete;

    template<typename T>
    static T clamp(const T& lo, const T& hi, const T& val)
    { 
        return std::min(std::max(0.0, val), hi);
    }

    /// https://stackoverflow.com/questions/7706339/grayscale-to-red-green-blue-matlab-jet-color-scale
    static Color ColorMapValue(double v, double vmin = 0.0, double vmax = 1.0)
    {
        Color c (1.0,1.0,1.0); // white

        if (v < vmin) { v = vmin; }
        if (v > vmax) { v = vmax; }

        double dv = vmax - vmin;

        if (v < (vmin + 0.25 * dv)) 
        {
            c.r = 0;
            c.g = 4 * (v - vmin) / dv;
        }
        else if (v < (vmin + 0.5 * dv)) 
        {
            c.r = 0;
            c.b = 1 + 4 * (vmin + 0.25 * dv - v) / dv;
        }
        else if (v < (vmin + 0.75 * dv))
        {
            c.r = 4 * (v - vmin - 0.5 * dv) / dv;
            c.b = 0;
        }
        else 
        {
            c.g = 1 + 4 * (vmin + 0.75 * dv - v) / dv;
            c.b = 0;
        }

        return c;
    }
};

class Terrain;

class TerrainApp : public AppTime
{
public:
    TerrainApp();

    int init()   override;
    int quit()   override;
    int render() override;
    int update(const float time, const float delta) override;
    std::unique_ptr<Terrain> terrain;

protected:
    Orbiter m_Camera;
    GLuint m_Program;
};

class TerrainData
{
public:
    enum SF
    {
        HEIGHT, SLOPE, LAPLACIAN, DRAIN, NUM_SCALAR_FIELDS
    };

    const ScalarField& GetHeightSF()    const { return scalarField[SF::HEIGHT]; }
    const ScalarField& GetSlopeSF()     const { return scalarField[SF::SLOPE]; }
    const ScalarField& GetLaplacianSF() const { return scalarField[SF::LAPLACIAN]; }
    const ScalarField& GetDrainageSF()  const { return scalarField[SF::DRAIN]; }

    ScalarField& GetHeightSF()    { return scalarField[SF::HEIGHT];    }
    ScalarField& GetSlopeSF()     { return scalarField[SF::SLOPE];     }
    ScalarField& GetLaplacianSF() { return scalarField[SF::LAPLACIAN]; }
    ScalarField& GetDrainageSF()  { return scalarField[SF::DRAIN]; }

    const std::array<ScalarField, 4>& GetArray() { return scalarField;  }
protected:
    std::array<ScalarField, 4> scalarField;

};

class Terrain
{
public:
    Terrain() 
    {
        //m_objet     = read_indexed_mesh("data/sphere.obj");
        //m_objet     = read_indexed_mesh("data/grass/grass.obj");
        m_sapin     = read_mesh("data/tree/tree.obj");
        m_tex_leaf = read_texture(0, "data/tree/diffuse.jpeg");
        m_sphere = read_mesh("data/sphere.obj");
        m_tex_rocks = read_texture(0, "data/bush/bush.png");
        m_rocks = read_mesh("data/bush/bush.obj");

        //m_tex_bark = read_texture(1, "data/tree/bark.png");

        //m_objet     = read_mesh("data/sphere.obj");
        model = Scale(1, 1, 1);
        m_vegetation_prog = read_program("data/shaders/vegetation.glsl");
        m_terrain_program = read_program("data/shaders/terrain.glsl");
        program_print_errors(m_vegetation_prog);
        program_print_errors(m_terrain_program);
    }
    TerrainData data;
    void Polygonize();
    Mesh& GetMesh() { return mesh;  }
    Transform& GetModel() { return model; }
    void ComputeDrainage();
    void SimulateErosion();

    void ProceduralPlacement(float radius = 1.0f, Vector regionSize = Vector(1, 0, 1), int numTries = 30);
    Mesh mesh;
    std::vector<Vector> posVegetation;
    Mesh m_sapin;
    Mesh m_sphere;
    Mesh m_rocks;
    GLuint m_vao_terrain;

    GLuint m_vao_veg;
    GLuint m_vbo_instance;
    GLuint m_vegetation_prog;
    GLuint m_terrain_program;
    GLuint m_tex_leaf, m_tex_rocks;
public:
    void InitFromTexture(const Image& im);
public:
    void StreamPowerErosion(double k); 
    void HillSlopeErosion(double k);
protected:
    Transform model;
};


int main(int argc, char** argv)
{
    TerrainApp tp;
    tp.run();

    return 0;
}