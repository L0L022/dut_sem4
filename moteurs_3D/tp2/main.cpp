#include <osgViewer/Viewer>
#include <osg/ShapeDrawable>
#include <osg/Material>
#include <osg/PositionAttitudeTransform>
#include <osgGA/NodeTrackerManipulator>
#include <osgGA/GUIEventHandler>
#include <iostream>

class SearchNode : public osg::NodeVisitor
{
public:
    SearchNode(const std::string &name) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN), _name(name) {}

    virtual void apply(osg::Node &n) {
        if (n.getName() == _name)
            _node = &n;
        else
            traverse(n);
    }

    inline osg::Node *node() const { return _node; }

protected:
    std::string _name;
    osg::ref_ptr<osg::Node> _node;
};

class EventHandler : public osgGA::GUIEventHandler
{
public:
    EventHandler(osg::ref_ptr<osg::StateSet> state, osg::ref_ptr<osg::Node> scene) : _state(state), _scene(scene), move_name(" ") {}

    virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa) {
        auto direction = osg::Vec3d{0, 0, 0};

        switch (ea.getEventType()) {
        case osgGA::GUIEventAdapter::KEYDOWN:
            switch (ea.getKey()) {
            case '1':
                switchLight(GL_LIGHT1);
                break;
            case '2':
                switchLight(GL_LIGHT2);
                break;
            case 'b':
                move_name = "patBox";
                break;
            case 's':
                move_name = "patSphere";
                break;
            case 'c':
                move_name = "patCone";
                break;
            case osgGA::GUIEventAdapter::KEY_Up:
                direction[1] += 1;
                break;
            case osgGA::GUIEventAdapter::KEY_Down:
                direction[1] -= 1;
                break;
            case osgGA::GUIEventAdapter::KEY_Right:
                direction[0] += 1;
                break;
            case osgGA::GUIEventAdapter::KEY_Left:
                direction[0] -= 1;
                break;
            case osgGA::GUIEventAdapter::KEY_Page_Up:
                direction[2] += 1;
                break;
            case osgGA::GUIEventAdapter::KEY_Page_Down:
                direction[2] -= 1;
                break;
            default:
                break;
            }
            break;

        default:
            break;
        }

        SearchNode search(move_name);
        _scene->accept(search);
        auto n = search.node();
        if (n != nullptr) {
            auto pos = n->asTransform()->asPositionAttitudeTransform()->getPosition();
            n->asTransform()->asPositionAttitudeTransform()->setPosition(pos + direction);
        }

        return false;
    }

private:
    void switchLight(int light) {
        _state->setMode(light, _state->getMode(light) == osg::StateAttribute::ON ? osg::StateAttribute::OFF : osg::StateAttribute::ON);
    }

private:
    osg::ref_ptr<osg::StateSet> _state;
    osg::ref_ptr<osg::Node> _scene;
    std::string move_name;
};

osg::Group* creation_troupeau(int nb_vaches, float taillex, float tailley)
{
    osg::ref_ptr<osg::Group> scene = new osg::Group;
    for (size_t i = 0; i < nb_vaches; ++i) {
        osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
        pat->setPosition({i/taillex, i/tailley, 0});
        scene->addChild(pat);
    }
    return scene;
}

int main(int argc, char *argv[])
{
    osg::DisplaySettings::instance()->setNumMultiSamples(8);

    osg::ref_ptr<osg::Shape> shapeBox = new osg::Box({0, 0, 0}, 2, 3, 4);
    osg::ref_ptr<osg::Material> materialBox = new osg::Material;
    materialBox->setAmbient(osg::Material::FRONT_AND_BACK, {0.5, 0, 0, 1});
    materialBox->setDiffuse(osg::Material::FRONT_AND_BACK, {0.9, 0, 0, 1});
    materialBox->setSpecular(osg::Material::FRONT_AND_BACK, {0.2, 0.2, 0.2, 1});
    materialBox->setShininess(osg::Material::FRONT_AND_BACK, 64);
    osg::ref_ptr<osg::ShapeDrawable> shapeDrawableBox = new osg::ShapeDrawable(shapeBox);
    shapeDrawableBox->getOrCreateStateSet()->setAttributeAndModes(materialBox);
    osg::ref_ptr<osg::Geode> geodeBox = new osg::Geode;
    geodeBox->addDrawable(shapeDrawableBox);

    osg::ref_ptr<osg::PositionAttitudeTransform> patBox = new osg::PositionAttitudeTransform;
    patBox->setName("patBox");
    patBox->setPosition({0, 0, 0});
    patBox->addChild(geodeBox);

    osg::ref_ptr<osg::Shape> shapeSphere = new osg::Sphere({0, 0, 0}, 1);
    osg::ref_ptr<osg::Material> materialSphere = new osg::Material;
    materialSphere->setAmbient(osg::Material::FRONT_AND_BACK, {0.5, 0.5, 0, 1});
    materialSphere->setDiffuse(osg::Material::FRONT_AND_BACK, {1, 1, 0, 1});
    materialSphere->setSpecular(osg::Material::FRONT_AND_BACK, {0.2, 0.2, 0.2, 1});
    materialSphere->setShininess(osg::Material::FRONT_AND_BACK, 64);
    osg::ref_ptr<osg::ShapeDrawable> shapeDrawableSphere = new osg::ShapeDrawable(shapeSphere);
    shapeDrawableSphere->getOrCreateStateSet()->setAttributeAndModes(materialSphere);
    osg::ref_ptr<osg::Geode> geodeSphere = new osg::Geode;
    geodeSphere->addDrawable(shapeDrawableSphere);

    osg::ref_ptr<osg::PositionAttitudeTransform> patSphere = new osg::PositionAttitudeTransform;
    patSphere->setName("patSphere");
    patSphere->setPosition({5, 0, 0});
    patSphere->addChild(geodeSphere);

    osg::ref_ptr<osg::Shape> shapeCone = new osg::Cone({0, 0, 0}, 1, 2);
    osg::ref_ptr<osg::Material> materialCone = new osg::Material;
    materialCone->setAmbient(osg::Material::FRONT_AND_BACK, {0.5, 0, 0.5, 1});
    materialCone->setDiffuse(osg::Material::FRONT_AND_BACK, {1, 0, 1, 1});
    materialCone->setSpecular(osg::Material::FRONT_AND_BACK, {0.2, 0.2, 0.2, 1});
    materialCone->setShininess(osg::Material::FRONT_AND_BACK, 64);
    osg::ref_ptr<osg::ShapeDrawable> shapeDrawableCone = new osg::ShapeDrawable(shapeCone);
    shapeDrawableCone->getOrCreateStateSet()->setAttributeAndModes(materialCone);
    osg::ref_ptr<osg::Geode> geodeCone = new osg::Geode;
    geodeCone->addDrawable(shapeDrawableCone);

    osg::ref_ptr<osg::PositionAttitudeTransform> patCone = new osg::PositionAttitudeTransform;
    patCone->setName("patCone");
    patCone->setPosition({0, 5, 0});
    patCone->addChild(geodeCone);

    osg::ref_ptr<osg::LightSource> light1 = new osg::LightSource;
    light1->getLight()->setLightNum(1);
    light1->getLight()->setPosition({1, 1, 1, 0});
    light1->getLight()->setAmbient({0.5, 0.5, 0.5, 1});
    light1->getLight()->setDiffuse({1, 1, 1, 1});
    light1->getLight()->setSpecular({0.2, 0.2, 0.2, 1});

    osg::ref_ptr<osg::LightSource> light2 = new osg::LightSource;
    light2->getLight()->setLightNum(2);
    light2->getLight()->setPosition({1, 1, 1, 1});
    light2->getLight()->setAmbient({0, 0.5, 0.5, 1});
    light2->getLight()->setDiffuse({0, 1, 1, 1});
    light2->getLight()->setSpecular({0.2, 0.2, 0.2, 1});

    osg::ref_ptr<osg::Group> lights = new osg::Group;
    lights->addChild(light1);
    lights->addChild(light2);

    osg::ref_ptr<osg::Group> scene = new osg::Group;
    scene->addChild(lights);
    scene->addChild(patBox);
    scene->addChild(patSphere);
    scene->addChild(patCone);

    osg::ref_ptr<osg::StateSet> state = scene->getOrCreateStateSet();
    state->setMode(GL_LIGHT0, osg::StateAttribute::OFF);
    state->setMode(GL_LIGHT1, osg::StateAttribute::ON);
    state->setMode(GL_LIGHT2, osg::StateAttribute::ON);

    osg::ref_ptr<osgGA::NodeTrackerManipulator> manip = new osgGA::NodeTrackerManipulator;
    manip->setTrackNode(geodeCone);
    manip->setTrackerMode(osgGA::NodeTrackerManipulator::NODE_CENTER);

    osg::ref_ptr<EventHandler> eventHandler = new EventHandler(state, scene);

    osgViewer::Viewer viewer;
    viewer.setUpViewInWindow(100, 50, 800, 600);
    viewer.getCamera()->setClearColor({1, 1, 1, 1});
    viewer.setSceneData(scene);
    viewer.setCameraManipulator(manip);
    viewer.addEventHandler(eventHandler);

    osgViewer::Viewer::Windows windows;
    viewer.getWindows(windows);
    windows[0]->setWindowName("TP 1");

    return viewer.run();
}
