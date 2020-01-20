#include "mex.hpp"
#include "mexAdapter.hpp"
#include <algorithm>
#include <vector>
#include <queue>

#define BLACK 0
#define RED 1
#define GREEN 2

//double INF = std::numeric_limits<double>::max();
double INF = 3000;

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
    void sort()
	{
        std::make_heap(this->c.begin(), this->c.end(), this->comp);
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
        const size_t num_rows = F.getDimensions()[0];
        const size_t num_columns = F.getDimensions()[1];
        matlab::data::TypedArray<double> dist = factory.createArray<double>({ num_rows, num_columns });
        matlab::data::TypedArray<int> states = factory.createArray<int>({ num_rows, num_columns });
        fmm(F, src, t0, display, dist, states);
        outputs[0] = std::move(dist);
    }

    bool update(
        int x,
        int y,
        node_priority_queue& queue,
        matlab::data::TypedArray<double>& F,
        matlab::data::TypedArray<double>& dist,
        matlab::data::TypedArray<int>& states)
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
    	if(in_bounds(F, states, neighbor_x, neighbor_y))
    	{
            T_left = dist[neighbor_y][neighbor_x];
    	}
        else
        {
            T_left = INF;
        }

    	/**
    	 * T_right
    	 */
        double T_right;
        neighbor_x = node_x + 1;
        neighbor_y = node_y;
        if (in_bounds(F, states, neighbor_x, neighbor_y))
        {
            T_right = dist[neighbor_y][neighbor_x];
        }
        else
        {
            T_right = INF;
        }

    	/**
    	 * T_top
    	 */
        double T_top;
        neighbor_x = node_x;
        neighbor_y = node_y - 1;
        if (in_bounds(F, states, neighbor_x, neighbor_y))
        {
            T_top = dist[neighbor_y][neighbor_x];
        }
        else
        {
            T_top = INF;
        }

    	/**
    	 * T_bottom
    	 */
        double T_bottom;
        neighbor_x = node_x;
        neighbor_y = node_y + 1;
        if (in_bounds(F, states, neighbor_x, neighbor_y))
        {
            T_bottom = dist[neighbor_y][neighbor_x];
        }
        else
        {
            T_bottom = INF;
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

    	if(states[node.y][node.x] == GREEN)
    	{
            states[node.y][node.x] = RED;
            queue.push(node);
            return false;
    	}

        return t < node.GetTime();

		//std::ostringstream stream;
		//stream << "(" << node.x << ", " << node.y << "): " << node.GetTime() << std::endl;
		//displayOnMATLAB(std::move(stream));
    }

    bool in_bounds(
        matlab::data::TypedArray<double>& F,
        matlab::data::TypedArray<int>& states,
        int x,
        int y)
    {
        const size_t rows = F.getDimensions()[0];
        const size_t cols = F.getDimensions()[1];

        if((x >= 0) && (x < cols) && (y >= 0) && (y < rows))
        {
            return states[y][x] != BLACK;
        }

        return false;
    }

	void render(
        matlab::data::TypedArray<double>& dist,
        matlab::data::TypedArray<int>& states,
        int iteration)
    {
    	// Call figure only once (so we won't have endless figure windows all over the places)
        if (iteration == 0)
        {
            matlabPtr->eval(u"figure");
            matlabPtr->eval(u"drawnow limitrate");
        }

        matlabPtr->eval(u"handle1 = subplot(2,1,1)");
        std::vector<matlab::data::Array> imagesc_args({ dist });
        matlabPtr->feval(u"imagesc", imagesc_args);
        std::vector<matlab::data::Array> colormap_args1({ matlabPtr->getVariable(u"handle1"), factory.createCharArray("parula") });
        matlabPtr->feval(u"colormap", colormap_args1);
        matlabPtr->eval(u"drawnow limitrate");

        matlabPtr->eval(u"handle2 = subplot(2,1,2)");
        std::vector<matlab::data::Array> image_args({ states });
        matlabPtr->feval(u"image", image_args);
        matlabPtr->eval(u"forest_colors = [0, 0, 0; 1, 0, 0; 0, 1, 0]");
        std::vector<matlab::data::Array> colormap_args2({ matlabPtr->getVariable(u"handle2"), matlabPtr->getVariable(u"forest_colors") });
        matlabPtr->feval(u"colormap", colormap_args2);
        matlabPtr->eval(u"drawnow limitrate");
    }

    void fmm(
        matlab::data::TypedArray<double>& F,
        matlab::data::TypedArray<double>& src,
        matlab::data::TypedArray<double>& t0,
        matlab::data::CharArray& display,
        matlab::data::TypedArray<double>& dist,
        matlab::data::TypedArray<int>& states)
	{
        const size_t rows = F.getDimensions()[0];
        const size_t cols = F.getDimensions()[1];

        node_priority_queue queue;
        queue.push(Node(src[0], src[1], &dist));

        for(size_t i = 0; i < rows; i++)
        {
            for (size_t j = 0; j < cols; j++)
            {
                dist[i][j] = INF;
                states[i][j] = GREEN;
            }
        }

        Node top_node = queue.top();
        dist[top_node.y][top_node.x] = t0[0];
        states[top_node.y][top_node.x] = RED;	
        std::vector<std::pair<int, int>> deltas = { {1,0}, {-1,0} ,{0,1} , {0,-1} };

        int iteration = 0;
    	while(!queue.empty())
    	{
            if (display.toAscii() != "silent")
            {
                if (iteration % 3000 == 0)
                {
                    render(dist, states, iteration);
                }
            }
    		
            Node top_node = queue.top();
            queue.pop();

            bool needs_sort = false;
    		
            for(auto delta : deltas)
            {
                int x_delta = delta.first;
                int y_delta = delta.second;
                int curr_x = top_node.x + x_delta;
                int curr_y = top_node.y + y_delta;
                if(in_bounds(F, states, curr_x, curr_y))
                {
                    if(update(curr_x, curr_y, queue, F, dist, states))
                    {
                        needs_sort = true;
                    }
                }
            }

            states[top_node.y][top_node.x] = BLACK;

            if (needs_sort)
            {
                queue.sort();
            }
    		
            iteration++;
    	}

        render(dist, states, iteration);
    }
};
