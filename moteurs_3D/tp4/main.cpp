#include <osgViewer/Viewer>
#include <osg/ShapeDrawable>
#include <osg/Material>
#include <osg/PositionAttitudeTransform>
#include <osgGA/NodeTrackerManipulator>
#include <osgGA/GUIEventHandler>
#include <osgDB/ReadFile>
#include <osg/Texture2D>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Switch>
#include <osg/Camera>
#include <osg/Fog>
#include <osgParticle/PrecipitationEffect>
#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowMap>
#include <osgShadow/SoftShadowMap>
#include <osgGA/DriveManipulator>
#include <osg/NodeCallback>
#include <osgUtil/LineSegmentIntersector>
#include <osg/TexMat>
#include <iostream>
#include <random>

osg::Node* creation_terrain()
{
    osg::ref_ptr<osg::Image> image = osgDB::readImageFile("terrain.tga");

    osg::ref_ptr<osg::HeightField> field = new osg::HeightField;
    field->allocate(image->s(), image->t());
    for (size_t y = 0; y < image->t(); ++y)
        for (size_t x = 0; x < image->s(); ++x)
            field->setHeight(x, y, (*image->data(x, y) / 255.f) * 30.f);

    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    texture->setImage(osgDB::readImageFile("herbe.tga"));
    texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

    osg::Matrixd matrix;
    matrix.makeScale(10, 10, 1);

    osg::ref_ptr<osg::TexMat> matTexture = new osg::TexMat;
    matTexture->setMatrix(matrix);

    osg::ref_ptr<osg::ShapeDrawable> shapeDrawable = new osg::ShapeDrawable(field);
    shapeDrawable->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
    shapeDrawable->getOrCreateStateSet()->setTextureAttributeAndModes(0, matTexture, osg::StateAttribute::ON);

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable(shapeDrawable);

    return geode.release();
}

bool intersection_terrain(float x, float y, osg::Node * terrain, osg::Vec3 & pos, osg::Vec3 & normale)
{
    osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(osg::Vec3(x, y, 1000), osg::Vec3(x, y, -1000));
    osgUtil::IntersectionVisitor iv(intersector);
    terrain->accept(iv);
    if (intersector->containsIntersections()) {
        osgUtil::LineSegmentIntersector::Intersections & intersections = intersector->getIntersections();
        auto intersection = *intersections.begin();
        pos = intersection.getWorldIntersectPoint();
        normale = intersection.getWorldIntersectNormal();
        return true;
    }
    return false;
}

osg::Group * creation_foret(osg::Node * terrain, size_t nb_arbres)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> disX(terrain->asGeode()->getBoundingBox().xMin(), terrain->asGeode()->getBoundingBox().xMax());
    std::uniform_real_distribution<float> disY(terrain->asGeode()->getBoundingBox().yMin(), terrain->asGeode()->getBoundingBox().yMax());
    std::uniform_real_distribution<float> disTaille(5, 20);


    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    texture->setImage(osgDB::readImageFile("arbre.tga"));
    texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
    texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);


    osg::ref_ptr<osg::Group> foret = new osg::Group;

    for (size_t i = 0; i < nb_arbres; ++i) {
        float taille = disTaille(gen);

        osg::ref_ptr<osg::Geometry> quad = osg::createTexturedQuadGeometry({-5, 0, 0}, {taille, 0, 0}, {0, 0, taille}, 0, 0, 1, 1);
        quad->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
        quad->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
        quad->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

        osg::ref_ptr<osg::Billboard> billboard = new osg::Billboard;
        billboard->setMode(osg::Billboard::AXIAL_ROT);
        billboard->setAxis({0, 0, 1});
        billboard->addDrawable(quad);
        billboard->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

        osg::Vec3 pos, normale;
        intersection_terrain(disX(gen), disY(gen), terrain, pos, normale);

        osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
        pat->setPosition(pos);
        pat->addChild(billboard);
        foret->addChild(pat);
    }

    return foret.release();
}

osg::Group * creation_foret(osg::Node * terrain, const std::string &fileName)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> disTaille(5, 20);

    osg::ref_ptr<osg::Image> image = osgDB::readImageFile(fileName);

    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    texture->setImage(osgDB::readImageFile("arbre.tga"));
    texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
    texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);

    osg::ref_ptr<osg::Group> foret = new osg::Group;

    for (size_t y = 0; y < image->t(); ++y) {
        for (size_t x = 0; x < image->s(); ++x) {
            if (*image->data(x, y) != 0) {
                float taille = disTaille(gen);

                osg::ref_ptr<osg::Geometry> quad = osg::createTexturedQuadGeometry({-5, 0, 0}, {taille, 0, 0}, {0, 0, taille}, 0, 0, 1, 1);
                quad->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
                quad->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
                quad->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

                osg::ref_ptr<osg::Billboard> billboard = new osg::Billboard;
                billboard->setMode(osg::Billboard::AXIAL_ROT);
                billboard->setAxis({0, 0, 1});
                billboard->addDrawable(quad);
                billboard->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

                osg::Vec3 pos, normale;
                intersection_terrain(x/float(image->s())*terrain->asGeode()->getBoundingBox().xMax(), y/float(image->t())*terrain->asGeode()->getBoundingBox().yMax(), terrain, pos, normale);

                osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
                pat->setPosition(pos);
                pat->addChild(billboard);
                foret->addChild(pat);
            }
        }
    }

    return foret.release();
}

osg::Node * creation_tank(osg::Node * terrain)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> disX(terrain->asGeode()->getBoundingBox().xMin(), terrain->asGeode()->getBoundingBox().xMax());
    std::uniform_real_distribution<float> disY(terrain->asGeode()->getBoundingBox().yMin(), terrain->asGeode()->getBoundingBox().yMax());

    osg::ref_ptr<osg::Node> tank = osgDB::readNodeFile("t72-tank/t72-tank_des.flt");

    osg::Vec3 pos, normale;
    intersection_terrain(disX(gen), disY(gen), terrain, pos, normale);

    osg::Quat rotation;
    rotation.makeRotate({0, 0, 1}, normale);

    osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
    pat->setPosition(pos);
    pat->setAttitude(rotation);
    pat->addChild(tank);

    return pat.release();
}

int main(int argc, char *argv[])
{
    osg::DisplaySettings::instance()->setNumMultiSamples(8);

    osg::ref_ptr<osg::Node> terrain = creation_terrain();

    osg::ref_ptr<osg::LightSource> light1 = new osg::LightSource;
    light1->getLight()->setLightNum(1);
    light1->getLight()->setPosition({1, 1, 1, 0});
    light1->getLight()->setAmbient({0.5, 0.5, 0.5, 1});
    light1->getLight()->setDiffuse({1, 1, 1, 1});
    light1->getLight()->setSpecular({0.2, 0.2, 0.2, 1});


    osg::ref_ptr<osg::Group> lights = new osg::Group;
    lights->addChild(light1);

    osg::ref_ptr<osgShadow::SoftShadowMap> st = new osgShadow::SoftShadowMap;
    st->setSoftnessWidth(0.001);

    osg::ref_ptr<osgShadow::ShadowedScene> shadowedScene = new osgShadow::ShadowedScene;
    shadowedScene->setShadowTechnique(st);
    shadowedScene->addChild(lights);
    shadowedScene->addChild(terrain);
    shadowedScene->addChild(creation_foret(terrain, "foret.tga"));
    shadowedScene->addChild(creation_tank(terrain));

    osg::ref_ptr<osg::Group> scene = new osg::Group;
    scene->addChild(shadowedScene);

    osg::ref_ptr<osg::StateSet> state = scene->getOrCreateStateSet();
    state->setMode(GL_LIGHT0, osg::StateAttribute::OFF);
    state->setMode(GL_LIGHT1, osg::StateAttribute::ON);

    osg::ref_ptr<osg::Camera> cam1 = new osg::Camera;
    cam1->getOrCreateStateSet()->setGlobalDefaults();
    cam1->setClearColor({0, 0, 0, 1});
    cam1->setProjectionMatrixAsPerspective(30, 4.f/3.f, 0.1, 1000);
    cam1->setViewMatrixAsLookAt({0, -10, 10}, {0, 0, 0}, {0, 0, 1});

    osgViewer::Viewer viewer;
    viewer.setCamera(cam1);
    viewer.setSceneData(scene);
    viewer.addEventHandler(new osgViewer::StatsHandler);
    viewer.setRunMaxFrameRate(30.0);
    viewer.setUpViewInWindow(100, 50, 800, 600);

    osgViewer::Viewer::Windows windows;
    viewer.getWindows(windows);
    windows[0]->setWindowName("TP 4");

    return viewer.run();
}
