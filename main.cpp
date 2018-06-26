#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <iomanip>
#include <fstream>
#define NULLPROBABILITY -1.0

class Node {
    protected:
        int value;
        float probability;

    public:
        Node() = default;
        Node(int value, float probability);
        void setValue(int value) { this->value = value; };
        void setProbability(float probability) { this->probability = probability; };
        int getValue() { return value; };
        float getProbability() { return probability; };
};

Node::Node(int value, float probability) {
    this->value = value;
    this->probability = probability;
}

class ElementsVectorHelper {
public:
    int getPositionOfMaxElement(std::vector<float> *vector);
    int getValueOfMaxElement(std::vector<float> *vector);
    float getMaxElement(std::vector<float> *vector);
    float selectMaxProbability(std::vector<float> *vector);
};

int ElementsVectorHelper::getPositionOfMaxElement(std::vector<float> *vector) {
    return std::distance(vector->begin(), std::max_element(vector->begin(), vector->end()));
}

// be careful on using this! this function will return the value for the model of the problem.
// thus, if the position of the max probability in the vector is 0, then the value returned is 1.
int ElementsVectorHelper::getValueOfMaxElement(std::vector<float> *vector) {
    return (std::distance(vector->begin(), std::max_element(vector->begin(), vector->end())) + 1);
}

float ElementsVectorHelper::getMaxElement(std::vector<float> *vector) {
    return *std::max_element(vector->begin(), vector->end());
}

// this function is calles select because it does not only returns the max element,
// which is straightforward to do, but also set the value on the array as -1.0 to allow
// next run to find a new max probability
float ElementsVectorHelper::selectMaxProbability(std::vector<float> *vector) {
    float maxProbability = getMaxElement(vector);
    vector->at(getPositionOfMaxElement(vector)) = NULLPROBABILITY;

    return maxProbability;
}

class TreeHelper {
    public:
        void setRoot(Node root, std::vector<Node> *tree);
        void setLeftChildNode(Node node, int parentPositionInVector, std::vector<Node> *tree);
        void setRightChildNode(Node node, int parentPositionInVector, std::vector<Node> *tree);
        int getPositionOfFirstNodeFound(Node node, std::vector<Node> *tree);
        bool nodeHasLeftChild(Node node, std::vector<Node> *tree);
        bool nodeHasRightChild(Node node, std::vector<Node> *tree);
        std::vector<Node> greedyTree(std::vector<float> tempArray, std::vector<Node> tree);
        float getExpectedCost(std::vector<Node> tree);
};

void TreeHelper::setRoot(Node root, std::vector<Node> *tree) {
    tree->at(0) = root;
}

void TreeHelper::setLeftChildNode(Node node, int parentPositionInVector, std::vector<Node> *tree) {
    tree->at((parentPositionInVector * 2) + 1) = node;
}

void TreeHelper::setRightChildNode(Node node, int parentPositionInVector, std::vector<Node> *tree) {
    tree->at((parentPositionInVector * 2) + 2) = node;
}

int TreeHelper::getPositionOfFirstNodeFound(Node nodeToBeSearch, std::vector<Node> *tree) {
    int positionToBeReturned = 0;

    for (Node node : *tree) {
        if (nodeToBeSearch.getValue() == node.getValue())
            break;
        positionToBeReturned++;
    }

    return positionToBeReturned;
}

bool TreeHelper::nodeHasLeftChild(Node node, std::vector<Node> *tree) {
    if (tree->at((getPositionOfFirstNodeFound(node, tree)*2) + 1).getValue() == 0) {
        return false;
    } else {
        return true;
    }
}

bool TreeHelper::nodeHasRightChild(Node node, std::vector<Node> *tree) {
    if (tree->at((getPositionOfFirstNodeFound(node, tree))*2 + 2).getValue() == 0) {
        return false;
    } else {
        return true;
    }
}

std::vector<Node> TreeHelper::greedyTree(std::vector<float> dataElements, std::vector<Node> tree) {
    TreeHelper treeHelper;
    ElementsVectorHelper arrayHelper;

    // set the root, first
    treeHelper.setRoot(
            Node(
                    arrayHelper.getPositionOfMaxElement(&dataElements),
                    arrayHelper.selectMaxProbability(&dataElements)
            ),
            &tree
    );

    for(float elementInData : dataElements) {
        // dummy use for zero Warnings!!
        if(elementInData) {}
        int currentValueAssociatedToMaxProbability = arrayHelper.getPositionOfMaxElement(&dataElements) + 1;
        for(Node currentNodeInTreeEvaluated : tree) {
            if ((currentValueAssociatedToMaxProbability < currentNodeInTreeEvaluated.getValue()) &&
                !treeHelper.nodeHasLeftChild(currentNodeInTreeEvaluated, &tree))
            {
                // we don't want to add the null probability (-1.0) as a node!!
                if(arrayHelper.getMaxElement(&dataElements) == NULLPROBABILITY)
                    break;
                treeHelper.setLeftChildNode(
                        Node(
                                currentValueAssociatedToMaxProbability,
                                arrayHelper.selectMaxProbability(&dataElements)
                        ),
                        treeHelper.getPositionOfFirstNodeFound(currentNodeInTreeEvaluated, &tree),
                        &tree);
                break;
            }
            if ((currentValueAssociatedToMaxProbability > currentNodeInTreeEvaluated.getValue()) &&
                !treeHelper.nodeHasRightChild(currentNodeInTreeEvaluated, &tree))
            {
                // we don't want to add the null probability (-1.0) as a node!!
                if(arrayHelper.getMaxElement(&dataElements) == NULLPROBABILITY)
                    break;
                treeHelper.setRightChildNode(
                        Node(
                                currentValueAssociatedToMaxProbability,
                                arrayHelper.selectMaxProbability(&dataElements)
                        ),
                        treeHelper.getPositionOfFirstNodeFound(currentNodeInTreeEvaluated, &tree),
                        &tree);
                break;
            }
        }
    }

    return tree;

}

float TreeHelper::getExpectedCost(std::vector<Node> tree) {
    int temporaryLevelOfTree = 1;
    float expectedCost = 0.0;
    for(Node currentNode : tree) {
        if(currentNode.getValue() != 0) {
            if (temporaryLevelOfTree == 1) {
                expectedCost += currentNode.getProbability() * (float)temporaryLevelOfTree;
            } else if (temporaryLevelOfTree == 2) {
                expectedCost += currentNode.getProbability() * (float)temporaryLevelOfTree;
            } else {
                int level = (log2((double)temporaryLevelOfTree) == 0) ? 1:std::ceil(log2((double)temporaryLevelOfTree));
                expectedCost += currentNode.getProbability() * (float)level;
            }


        }
        temporaryLevelOfTree++;
    }
    return expectedCost;
}

int main() {
    TreeHelper treeHelper;

    std::string fileDataLine;
    std::ifstream inFile;
    inFile.open("data.txt");

    if (inFile.fail()) {
        std::cerr << "Could not open data file!" << std::endl;
        inFile.close();
        return 0;
    }

    while (inFile.good()) {
        // there won't be more than 202 nodes addressed in the (vector)
        // 100 elements * 2 + 2 = 200 + 2 = 202
        std::vector<Node> tree(202);
        std::vector<float> probabilities;
        std::getline(inFile, fileDataLine);
        std::getline(inFile, fileDataLine);

        // getline() has been showing some undefined behavior.
        // therefore, we need to put the 'second line' in a temp file and then read it again.
        std::ofstream tempFile;
        tempFile.open("temp.txt");

        if (tempFile.fail()) {
            std::cerr << "Could not open temp file for writing!" << std::endl;
            inFile.close();
            return 0;
        }

        tempFile << fileDataLine;
        tempFile.close();

        std::ifstream inTempFile;

        if (inTempFile.fail()) {
            std::cerr << "Could not open temp file for reading!" << std::endl;
            inFile.close();
            return 0;
        }

        inTempFile.open("temp.txt");

        std::string tmpFrequencyValueInLine;
        while (std::getline(inTempFile, tmpFrequencyValueInLine, ' ')) {
            probabilities.push_back(std::stof(tmpFrequencyValueInLine));
        }
        inTempFile.close();


        tree = treeHelper.greedyTree(probabilities, tree);

        std::cout << std::fixed << std::setprecision(4) << treeHelper.getExpectedCost(tree) << std::endl;
    }


    return 0;
}