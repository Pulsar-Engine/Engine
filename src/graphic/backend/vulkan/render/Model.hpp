#ifndef MODEL_HPP_
    #define MODEL_HPP_

    #include "Vertex.hpp"
    #include <vector>
    #include <unordered_map>
    

class Model {
    public:
        Model(const char *path);
        ~Model();
        const std::vector<Vertex> &getVertices() const;
        const std::vector<uint32_t> &getIndices() const;
    protected:
    private:
        std::vector<Vertex> _vertices;
        std::vector<uint32_t> _indices;
};

#endif /* !MODEL_HPP_ */
