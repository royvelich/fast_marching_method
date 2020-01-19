#include "mex.hpp"
#include "mexAdapter.hpp"
#include <algorithm>
#include <vector>
#include <queue>

struct Node
{
	Node(int x, int y, matlab::data::TypedArray<double>* dist) :
		x(x),
		y(y),
        dist(dist)
	{
		
	}

	double GetTime()
	{
        return (*dist)[y][x];
	}

    void SetTime(double t)
    {
        (*dist)[y][x] = t;
    }
	
    int x;
    int y;
    matlab::data::TypedArray<double>* dist;
};

class CompareNode {
public:
    bool operator()(Node& n1, Node& n2)
    {
        if (n1.GetTime() > n2.GetTime())
        {
            return true;
        }
    	
        return false;
    }
};

class node_priority_queue : public std::priority_queue<Node, std::vector<Node>, CompareNode>
{
public:

    //bool remove(const Node& node_to_remove) {
    //    auto it = std::find_if(this->c.begin(), this->c.end(), [&](const Node& curr_node) {
    //        if
    //    });
    //    //if (it != this->c.end()) {
    //    //    this->c.erase(it);
    //    //    std::make_heap(this->c.begin(), this->c.end(), this->comp);
    //    //    return true;
    //    //}
    //    //else {
    //    //    return false;
    //    //}
    //}

    void sort()
	{
        std::make_heap(this->c.begin(), this->c.end(), this->comp);
    }

    bool exist(const Node& node)
    {
		auto it = std::find_if(this->c.begin(), this->c.end(), [&](const Node& curr_node) {
			if((node.x == curr_node.x) && (node.y == curr_node.y))
			{
                return true;
			}

            return false;
		});

        return it != this->c.end();
    }
};

class MexFunction : public matlab::mex::Function
{
private:
    std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr;
    matlab::data::ArrayFactory factory;
	
public:
    MexFunction()
    {
        matlabPtr = getEngine();
    }

    void displayOnMATLAB(std::ostringstream stream)
    {
	    matlab::data::ArrayFactory factory;
        matlabPtr->feval(u"fprintf", 0, std::vector<matlab::data::Array>
            ({ factory.createScalar(stream.str()) }));
    }
	
    void operator()(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs)
	{
        matlab::data::TypedArray<double> F = std::move(inputs[0]);
        matlab::data::TypedArray<double> src = std::move(inputs[1]);
        matlab::data::TypedArray<double> t0 = std::move(inputs[2]);
        matlab::data::CharArray display = std::move(inputs[3]);
        const size_t numRows = F.getDimensions()[0];
        const size_t numColumns = F.getDimensions()[1];
        matlab::data::TypedArray<double> dist = factory.createArray<double>({ numRows, numColumns });
        fmm(F, src, t0, display, dist);
        outputs[0] = std::move(dist);
    }

    void update(
        int x,
        int y,
        node_priority_queue& queue,
        matlab::data::TypedArray<double>& F,
        matlab::data::TypedArray<double>& dist)
    {
        Node node = Node(x, y, &dist);
        int node_x = node.x;
        int node_y = node.y;

        int neighbor_x;
        int neighbor_y;

    	/**
    	 * T_left
    	 */
        double T_left;
        neighbor_x = node_x - 1;
        neighbor_y = node_y;
    	if(in_bounds(F, neighbor_x, neighbor_y))
    	{
            T_left = dist[neighbor_y][neighbor_x];
    	}
        else
        {
            T_left = std::numeric_limits<double>::max();
        }

    	/**
    	 * T_right
    	 */
        double T_right;
        neighbor_x = node_x + 1;
        neighbor_y = node_y;
        if (in_bounds(F, neighbor_x, neighbor_y))
        {
            T_right = dist[neighbor_y][neighbor_x];
        }
        else
        {
            T_right = std::numeric_limits<double>::max();
        }

    	/**
    	 * T_top
    	 */
        double T_top;
        neighbor_x = node_x;
        neighbor_y = node_y - 1;
        if (in_bounds(F, neighbor_x, neighbor_y))
        {
            T_top = dist[neighbor_y][neighbor_x];
        }
        else
        {
            T_top = std::numeric_limits<double>::max();
        }

    	/**
    	 * T_bottom
    	 */
        double T_bottom;
        neighbor_x = node_x;
        neighbor_y = node_y + 1;
        if (in_bounds(F, neighbor_x, neighbor_y))
        {
            T_bottom = dist[neighbor_y][neighbor_x];
        }
        else
        {
            T_bottom = std::numeric_limits<double>::max();
        }

    	/**
    	 * Calculate t
    	 */
        double T1 = std::min(T_left, T_right);
        double T2 = std::min(T_bottom, T_top);
        double f = F[node.y][node.x];
        double t;
    	if(std::abs(T1 - T2) < f)
    	{
            t = (T1 + T2 + std::sqrt(2 * f * f - std::pow(T1 - T2, 2))) / 2;
    	}
        else
        {
            t = std::min(T1, T2) + f;
        }

        node.SetTime(std::min(node.GetTime(), t));

    	if(!queue.exist(node))
    	{
            queue.push(node);
    	}

        //std::ostringstream stream;
        //stream << "(" << node.x << ", " << node.y << "): " << node.GetTime() << std::endl;
        //displayOnMATLAB(std::move(stream));
    }

    bool in_bounds(
        matlab::data::TypedArray<double>& F,
        int x,
        int y)
    {
        const size_t rows = F.getDimensions()[0];
        const size_t cols = F.getDimensions()[1];

    	if((x >= 0) && (x < cols) && (y >= 0) && (y < rows))
    	{
            return F[y][x] == 1;
    	}

        return false;
    }

    void fmm(
        matlab::data::TypedArray<double>& F,
        matlab::data::TypedArray<double>& src,
        matlab::data::TypedArray<double>& t0,
        matlab::data::CharArray& display,
        matlab::data::TypedArray<double>& dist)
	{
        const size_t rows = F.getDimensions()[0];
        const size_t cols = F.getDimensions()[1];

        node_priority_queue queue;
        queue.push(Node(src[0], src[1], &dist));

        for(size_t i = 0; i < rows; i++)
        {
            for (size_t j = 0; j < cols; j++)
            {
                dist[i][j] = std::numeric_limits<double>::max();
            }
        }

        Node top_node = queue.top();
        dist[top_node.y][top_node.x] = t0[0];

        std::vector<std::pair<int, int>> deltas = { {1,0}, {-1,0} ,{0,1} , {0,-1} };
    	
    	while(!queue.empty())
    	{
            Node top_node = queue.top();
            queue.pop();

            for(auto delta : deltas)
            {
                int x_delta = delta.first;
                int y_delta = delta.second;
                int curr_x = top_node.x + x_delta;
                int curr_y = top_node.y + y_delta;
                if(in_bounds(F, curr_x, curr_y))
                {
                    update(curr_x, curr_y, queue, F, dist);
                }
            }

            F[top_node.y][top_node.x] = 0;
            queue.sort();
    	}
    }
};
