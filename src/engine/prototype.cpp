#include "engine.hpp"

/*
TODO
object automated motion and movement/camera constraints
input abstraction for user specified keybinds
network communication updated for HTTPS
particle system
*/

class movement {
    /*
    Velocity calculation

    v = v0 + gt

    v0 initial velocity (m/s)
    t  time (s)
    g  gravitational acceleration (9.80665 m/s2)
    */

    float gravity;
};


//=====================================================================================================

class console : public main::scene {
public:
    bool load() {
        gui->create(&main::debug(), 512, 720, 0, 0, 0, 80).position(graphics->width() - 512 - 20, 20);
        main::debug().selectable = true;
        main::debug().alignment = platform::interface::widget::positioning::bottom;

        main::debug().events.handler(platform::input::POINTER, platform::input::WHEEL, [](const platform::input::event& ev) {
            // TODO: scroll up and down the contents
        }, 0);

        gui->create(&commandline, 512, 20, 0, 0, 0, 80).position(graphics->width() - 512 - 20, 750);
        commandline.selectable = true;
        commandline.input = true;
        commandline.multiline = false;
        commandline.content.limit = 1; // TODO: This should be covered by the multiline flag, but currently isn't.
        commandline.events.handler(platform::input::KEY, platform::input::DOWN, [this](const platform::input::event& ev) {
            std::vector<std::string> content;
            switch (ev.identifier) {
            case(38):
                commandline.content.position(1);
                break;
            case(40):
                commandline.content.position(-1);
                break;
            case(8): // Backspace to remove a character
                commandline.content.truncate(1);
                break;
            case(13): // Enter to submit
                content = commandline.content.get();
                if (content.size() && content.back().size() && content.back()[0] == '/') {
                    main::global().call(content.back());
                }
                else {
                    main::debug().content.add(content.back());
                }
                if (commandline.content.position() == 0) {
                    commandline.content.add();
                }
                commandline.content.position(commandline.content.size() * -1);
                break;
            default: // Every other printable gets added to the contents
                commandline.content.append(input->printable(ev.identifier));
            };
        });

        // Start off with an empty command
        commandline.content.add();

        return true;
    }

    void start() {
        main::debug().visible = true;
        gui->select(&commandline);
        commandline.visible = true;
        gui->position();
    }

    void stop() {
        gui->select(NULL);
        main::debug().visible = false;
        commandline.visible = false;
    }

    platform::interface::textbox commandline;
};

class splash : public main::scene {
public:
    void run() {
        if (start == 0) {
            start = time(NULL);
        }
        if (time(NULL) - start > 1) {
            main::global().transition("splash", "title");
        }
    }

    time_t start = 0;
};

class title : public main::scene {
public:

    bool load() {
        main::global().callback("perspective.fov", [](main::parameters_t p)->value_t { 
            int fov = main::global().has("perspective.fov") ? std::get<int>(main::global().get("perspective.fov")) : 90;
            graphics->projection(fov);
            return fov;
        });
        main::global().callback("blur.scale", [](main::parameters_t p)->value_t {
            float scale = main::global().has("blur.scale") ? std::get<double>(main::global().get("blur.scale")) : 0.5;
            graphics->dimensions(graphics->width(), graphics->height(), scale);
            return scale;
        });

        main::global().call("/set debug.input 0");

        main::global().call("/set ambient.position (10,10,10)");
        main::global().call("/set ambient.lookat (0,0,0)");
        main::global().call("/set ambient.color (0.4,0.4,0.4)");
        main::global().call("/set ambient.strength 0.8");
        main::global().call("/set ambient.bias 0.0055");

        main::global().call("/set blur.strength 4.0");
        main::global().call("/set blur.scale 0.5");

        main::global().call("/set dof.near 0.4");
        main::global().call("/set dof.far 0.05");

        main::global().call("/set edge.threshold 0.1");

        main::global().call("/set box1.position (0,-8,0)");
        main::global().call("/set box2.position (5,-7.7,0)");

        main::global().call("/set wiggle.position (-5,-7,0)");

        main::global().call("/set wiggle.surge 0.0");
        main::global().call("/set wiggle.sway 0.0");
        main::global().call("/set wiggle.heave 0.0");

        main::global().call("/set perspective.fov 90");

        main::global().call("/set shadow.scale 64");
        main::global().call("/set shadow.depth 100.0");

        main::global().call("/load sound raw/glados");

        main::global().call("/load shader shaders/basic basic");
        main::global().call("/load shader shaders/cell cell");
        main::global().call("/load shader shaders/defuse defuse");
        main::global().call("/load shader shaders/shadowmap shadowmap");
        main::global().call("/load shader shaders/depth_to_color depth_to_color");
        main::global().call("/load shader shaders/basic gui");
        main::global().call("/load shader shaders/basic skybox");
        main::global().call("/load shader shaders/cell objects");
        main::global().call("/load shader shaders/defuse_with_shadows scenery");
        main::global().call("/load shader shaders/wireframe wireframe");
        main::global().call("/load shader shaders/outline outline");
        main::global().call("/load shader shaders/depth depth");
        main::global().call("/load shader shaders/post post");
        main::global().call("/load shader shaders/blur blur");
        main::global().call("/load shader shaders/picking picking");

        main::global().call("/load font fonts/consolas-22 default");

        main::global().call("/load object drawable/marvin.png icon");

        main::global().call("/compile");

        gui->shader = assets->get<type::program>("gui");
        gui->font = assets->get<type::font>("default");

        gui->create(&main::global().progress(), graphics->width() / 2, 20, 0, 0, 0, 80).position(graphics->width() / 2 / 2, graphics->height() - 80);

        gui->create(&enter, 256, 256, 0, 0, 0, 80).position(20, 20);
            
        enter.events.handler(platform::input::POINTER, platform::input::MOVE, [](const platform::input::event& ev) {
            if (main::global().flag("debug.input")) {
                std::stringstream ss;
                ss << "hover_over(" << ev.point.x << ", " << ev.point.y << ")";
                main::debug().content.add(ss.str());
            }
            gui->print(210, 210, "HelloWorld"); // TODO: this won't draw... likely before or after the frame buffer swap, don't intend to ever do this anyway
            }, 1);

        enter.events.handler(platform::input::POINTER, platform::input::DOWN, [](const platform::input::event& ev) {
            if (main::global().flag("debug.input")) {
                std::stringstream ss;
                ss << "button_down(" << ev.point.x << ", " << ev.point.y << ")";
                main::debug().content.add(ss.str());
            }
            client->connect();
            //audio->start(main::global().sounds["glados"]);
            main::global().progress().visible = true;
            main::global().transition("title", "game");
        }, 1);

        main::global().toggle("debug"); // no current way to turn this on in android... so just start with it.

        // Forcing the state change
        main::global().transition("title", "game");

        return true;
    }

    void start() {
        enter.visible = true;
        enter.enabled = true;

        main::global().progress().visible = false;

        gui->position();

    }
    void run() {
        spatial::matrix frame;
        frame.identity();
        frame.translate(20, graphics->height() - 20 - 256, 0);

        graphics->draw(assets->get<type::object>("icon"), assets->get<type::program>("gui"), graphics->ortho, spatial::matrix(), frame);
    }
    void stop() {
        enter.visible = false;
        enter.enabled = false;

        main::global().progress().visible = false;
    }

    platform::interface::button enter;
};

class game : public main::scene {
public:
    type::object xAxis;
    type::object yAxis;
    type::object zAxis;

    type::object skybox;
    type::object box;
    type::object ground;

    type::object monkey;

    type::object ray;

    type::object bounds;

    std::vector<type::object> rays;

    bool load() {
        // TODO: don't use the progress bar to determine thread completion
        if (main::global().progress().value.get() == 0) {
            main::global().progress().value.set(1);
            std::thread([this]{
                xAxis = spatial::ray(spatial::vector(0.0, 0.0, 0.0), spatial::vector(2.0, 0.0, 0.0));
                xAxis.texture.create(1, 1, 255, 0, 0, 255);
                xAxis.xy_projection(0, 0, 1, 1);

                main::global().progress().value.set(10);

                yAxis = spatial::ray(spatial::vector(0.0, 0.0, 0.0), spatial::vector(0.0, 2.0, 0.0));
                yAxis.texture.create(1, 1, 0, 255, 0, 255);
                yAxis.xy_projection(0, 0, 1, 1);

                main::global().progress().value.set(20);

                zAxis = spatial::ray(spatial::vector(0.0, 0.0, 0.0), spatial::vector(0.0, 0.0, 2.0));
                zAxis.texture.create(1, 1, 0, 0, 255, 255);
                zAxis.xy_projection(0, 0, 1, 1);

                main::global().progress().value.set(30);
                assets->retrieve("objects/skybox.obj") >> format::parser::obj >> skybox;

                main::global().progress().value.set(40);
                assets->retrieve("objects/untitled.obj") >> format::parser::obj >> box;

                main::global().progress().value.set(50);
                assets->retrieve("objects/ground/ground.obj") >> format::parser::obj >> ground;

                main::global().progress().value.set(60);
                assets->retrieve("objects/monkey.obj") >> format::parser::obj >> monkey;

                // Just to simulate when more resources are required
                main::global().progress().value.set(70);

                main::global().call("/load entity objects/wiggle");

                //main::global().progress().value.set(80);
                //utilities::sleep(1000);
                //main::global().progress().value.set(90);
                //utilities::sleep(1000);
                main::global().progress().value.set(100);
             }).detach();
        }
        if (main::global().progress().value.get() < 100) {
            return false;
        }

        bounds = spatial::quad(1.0, 1.0);
        graphics->compile(bounds);

        main::global().call("/compile");

        main::global().call("/show entities");
        main::global().call("/play objects/wiggle idle");

        assets->get<type::entity>("objects/wiggle").allocate(3);
        assets->get<type::entity>("objects/wiggle").instances[1].position.reposition({ 3.0, 3.0, 3.0 });
        assets->get<type::entity>("objects/wiggle").instances[2].position.reposition({ 5.0, -3.0, 5.0 });
        assets->get<type::entity>("objects/wiggle").position();

        //assets->get<type::entity>("objects/wiggle").positions[0] = spatial::matrix().translate(0, -2, -10).scale(5.0f);
        //assets->get<type::entity>("objects/wiggle").positions[1] = spatial::matrix().translate(0, -2, -10).scale(5.0f);
        //assets->get<type::entity>("objects/wiggle").positions[2] = spatial::matrix().translate(0, -2, -10).scale(5.0f);

        //std::string mat = spatial::matrix().translate(0, -2, -10).scale(5.0f); // { {5,0,0,0}, {0,5,0,0}, {0,0,5,0}, {0,-2,-10,1} };

        graphics->compile(assets->get<type::entity>("objects/wiggle"));

        graphics->compile(xAxis);
        graphics->compile(yAxis);
        graphics->compile(zAxis);
        graphics->compile(skybox);
        graphics->compile(box);
        graphics->compile(ground);
        graphics->compile(monkey);

        ray = spatial::ray(spatial::vector(0.0, 0.0, -0.2), spatial::vector(0.0, 0.0, 0.2));
        ray.texture.create(1, 1, 255, 255, 255, 255);
        ray.xy_projection(0, 0, 1, 1);
        graphics->compile(ray);


        /*
        /// <summary>
        /// Just some random objects to play around with
        /// </summary>
        ray = spatial::ray(spatial::vector(0.0, 0.0, -0.2), spatial::vector(0.0, 0.0, 0.2));
        ray.texture.color.create(1, 1, 255, 255, 0, 255);
        ray.xy_projection(0, 0, 1, 1);
        graphics->compile(ray);

        trail = spatial::ray(spatial::vector(0.0, 0.0, -0.2), spatial::vector(0.0, 0.0, 0.2));
        trail.texture.color.create(1, 1, 255, 0, 0, 255);
        trail.xy_projection(0, 0, 1, 1);
        graphics->compile(trail);

        sphere = spatial::sphere(30, 30);
        sphere.texture.color.create(1, 1, 255, 255, 255, 255);
        sphere.xy_projection(0, 0, 1, 1);
        graphics->compile(sphere);

        visualized_bounds = bounds;
        visualized_bounds.texture.color.create(1, 1, 255, 255, 255, 255);
        visualized_bounds.xy_projection(0, 0, 1, 1);
        graphics->compile(visualized_bounds);
        */

        return true;
    }

    void draw(spatial::position pos, type::program& shader, const spatial::matrix& model, const spatial::matrix& view, const spatial::matrix& projection, unsigned int options = 0x00) {
        ray = spatial::ray(pos.center, pos.eye);
        ray.texture.color->create(1, 1, 255, 0, 0, 255);
        graphics->compile(ray);
        graphics->draw(ray, shader, projection, view, model, spatial::matrix(), options);

        ray = spatial::ray(pos.center, pos.up + pos.center);
        ray.texture.color->create(1, 1, 0, 255, 0, 255);
        graphics->compile(ray);
        graphics->draw(ray, shader, projection, view, model, spatial::matrix(), options);
    }

    void run() {
        auto& shader_basic = assets->get<type::program>("basic");
        auto& shader_shadowmap = assets->get<type::program>("shadowmap");
        auto& shader_defuse = assets->get<type::program>("defuse");
        auto& shader_skybox = assets->get<type::program>("skybox");
        auto& shader_objects = assets->get<type::program>("objects");
        auto& shader_scenery = assets->get<type::program>("scenery");
        auto& shader_wireframe = assets->get<type::program>("wireframe");
        auto& shader_outline = assets->get<type::program>("outline");
        auto& shader_depth = assets->get<type::program>("depth");
        auto& shader_post = assets->get<type::program>("post");
        auto& shader_blur = assets->get<type::program>("blur");
        auto& shader_picking = assets->get<type::program>("picking");

        auto& perspective = graphics->perspective;

        graphics->ambient.position.reposition(std::get<spatial::vector>(main::global().get("ambient.position")));
        graphics->ambient.position.lookat(std::get<spatial::vector>(main::global().get("ambient.lookat")));
        graphics->ambient.color = std::get<spatial::vector>(main::global().get("ambient.color"));
        graphics->ambient.bias = std::get<double>(main::global().get("ambient.bias"));
        graphics->ambient.strength = std::get<double>(main::global().get("ambient.strength"));

        graphics->parameter(0, std::get<double>(main::global().get("blur.strength")));
        graphics->parameter(1, std::get<double>(main::global().get("blur.scale")));
        graphics->parameter(2, std::get<double>(main::global().get("dof.near")));
        graphics->parameter(3, std::get<double>(main::global().get("dof.far")));
        graphics->parameter(4, std::get<double>(main::global().get("edge.threshold")));

        // View based on the camera
        spatial::matrix lighting = spatial::matrix().lookat(graphics->ambient.position.eye, graphics->ambient.position.center, graphics->ambient.position.up);
        spatial::matrix view = spatial::matrix().lookat(camera.eye, camera.center, camera.up);

        // orthographic view matrix relative to the target
        spatial::matrix ortho;
        auto scale = std::get<int>(main::global().get("shadow.scale"));
        auto depth = std::get<double>(main::global().get("shadow.depth"));

        ortho.ortho(scale, scale * -1.0f, scale, scale * -1.0f, 0.0f, depth);

        auto box1_matrix = spatial::matrix().translate(std::get<spatial::vector>(main::global().get("box1.position")));
        auto box2_matrix = spatial::matrix().translate(std::get<spatial::vector>(main::global().get("box2.position"))).scale(2.0f);

        auto wiggle_matrix = spatial::matrix().translate(std::get<spatial::vector>(main::global().get("wiggle.position")));

        assets->get<type::entity>("objects/wiggle").instances[2].position.surge(std::get<double>(main::global().get("wiggle.surge")));
        assets->get<type::entity>("objects/wiggle").instances[2].position.sway(std::get<double>(main::global().get("wiggle.sway")));
        assets->get<type::entity>("objects/wiggle").instances[2].position.heave(std::get<double>(main::global().get("wiggle.heave")));
        assets->get<type::entity>("objects/wiggle").position();

        graphics->compile(assets->get<type::entity>("objects/wiggle"));

        assets->get<type::entity>("objects/wiggle").animate();

        std::vector<std::vector<int>> building = {
            { 0 , 0 , 1, 1},
            { 0 , 0 , 1, 1},
            { 0 , 0 , 1, 1},
            { 1 , 1 , 1, 1},
            { 1 , 1 , 1, 1}
        };

        // Build the shadow map
        {
            auto scoped = graphics->target(graphics->shadow);

            graphics->clear();
            graphics->draw(ground, shader_shadowmap, ortho, lighting, spatial::matrix().scale(4.0f), ortho * lighting);
            graphics->draw(box, shader_shadowmap, ortho, lighting, box1_matrix, ortho * lighting);
            graphics->draw(box, shader_shadowmap, ortho, lighting, box2_matrix, ortho * lighting);
            graphics->draw(monkey, shader_shadowmap, ortho, lighting, spatial::matrix().translate(0, -2, -10).scale(5.0f), ortho * lighting);
            graphics->draw(assets->get<type::entity>("objects/wiggle"), shader_shadowmap, ortho, lighting, wiggle_matrix, ortho * lighting);
        }

        // Rendering to a texture, just to test the capability
        {
            // This entire scope will render to the poly texture, every frame... which is unnecessary, just testing for performance, etc.
            auto scoped = graphics->target(box);

            // just moving it a bit to move away from the edges
            spatial::matrix rendertotex;
            rendertotex.identity();
            rendertotex.translate(20, 20, 0);

            // orthographic view matrix relative to the target
            spatial::matrix ortho;
            ortho.ortho(0, box.texture.color->properties.width, 0, box.texture.color->properties.height);

            graphics->draw(assets->get<type::object>("icon"), shader_basic, ortho, spatial::matrix(), rendertotex);
        }

        // Render to the depth buffer for post processing
        {
            auto scoped = graphics->target(graphics->depth);

            graphics->clear();

            graphics->draw(box, shader_depth, perspective, view, box2_matrix);

            for (int y = 0; y < building.size(); y++) {
                for (int x = 0; x < building[y].size(); x++) {
                    if (building[y][x] > 0) {
                        auto pos = spatial::matrix().translate({ (float)x * 2.0f,(float)(building.size() - y) * 2.0f, 0 });
                        graphics->draw(box, shader_depth, perspective, view, pos);
                    }
                }
            }

            graphics->draw(monkey, shader_depth, perspective, view, spatial::matrix().translate(0, -2, -10).scale(5.0f));

            assets->get<type::entity>("objects/wiggle").flags = 1;
            graphics->draw(assets->get<type::entity>("objects/wiggle"), shader_depth, perspective, view, wiggle_matrix);
        }

        // Render to the picking buffer
        {
            auto scoped = graphics->target(graphics->picking);

            graphics->clear();

            double increment = 1.0 / 256;
            double current = 1.0;

            graphics->parameter(5, current);
            current -= increment;
            graphics->draw(box, shader_picking, perspective, view, box1_matrix);
            graphics->parameter(5, current);
            current -= increment;
            graphics->draw(box, shader_picking, perspective, view, box2_matrix);

            for (int y = 0; y < building.size(); y++) {
                for (int x = 0; x < building[y].size(); x++) {
                    if (building[y][x] > 0) {
                        auto pos = spatial::matrix().translate({ (float)x * 2.0f,(float)(building.size() - y) * 2.0f, 0 });
                        graphics->parameter(5, current);
                        current -= increment;
                        graphics->draw(box, shader_picking, perspective, view, pos);
                    }
                }
            }

            graphics->parameter(5, current);
            current -= increment;
            graphics->draw(monkey, shader_picking, perspective, view, spatial::matrix().translate(0, -2, -10).scale(5.0f));

            assets->get<type::entity>("objects/wiggle").flags = 1;
            graphics->parameter(5, current);
            current -= increment;
            graphics->draw(assets->get<type::entity>("objects/wiggle"), shader_picking, perspective, view, wiggle_matrix);
        }

        // Render to the color buffer
        {
            auto scoped = graphics->target(graphics->color);
            
            graphics->clear();

            graphics->draw(skybox, shader_skybox, perspective, view, spatial::matrix());

            graphics->draw(ground, shader_scenery, perspective, view, spatial::matrix().scale(4.0f), ortho * lighting, platform::graphics::render::NORMALS);

            for (int y = 0; y < building.size(); y++) {
                for (int x = 0; x < building[y].size(); x++) {
                    if (building[y][x] > 0) {
                        auto pos = spatial::matrix().translate({ (float)x * 2.0f,(float)(building.size() - y) * 2.0f, 0 });
                        graphics->draw(box, shader_objects, perspective, view, pos, ortho * lighting);
                    }
                }
            }

            graphics->draw(box, shader_objects, perspective, view, box1_matrix, ortho * lighting);
            graphics->draw(box, shader_objects, perspective, view, box2_matrix, ortho * lighting);

            graphics->draw(xAxis, shader_basic, perspective, view);
            graphics->draw(yAxis, shader_basic, perspective, view);
            graphics->draw(zAxis, shader_basic, perspective, view);

            graphics->draw(monkey, shader_objects, perspective, view, spatial::matrix().translate(0, -2, -10).scale(5.0f));

            draw(graphics->ambient.position, shader_basic, perspective, view, spatial::matrix());

            graphics->draw(assets->get<type::entity>("objects/wiggle"), shader_objects, perspective, view, wiggle_matrix);

            if (1) {
                spatial::matrix direct = spatial::position().lookat(camera.eye);
                bounds = direct.interpolate(spatial::quad(1.0, 1.0));
                graphics->compile(bounds);
                graphics->draw(bounds, shader_wireframe, perspective, view, spatial::matrix(), spatial::matrix(), platform::graphics::render::WIREFRAME);
            }
            else {
                graphics->draw(bounds, shader_wireframe, perspective, view, spatial::position().lookat(camera.eye), spatial::matrix(), platform::graphics::render::WIREFRAME);
            }

            for (auto& ray : rays) {
                graphics->draw(ray, shader_wireframe, perspective, view, spatial::matrix(), spatial::matrix(), platform::graphics::render::WIREFRAME);
            }

        }

        // Create the blur buffer from the color buffer and depth
        {
            auto scoped = graphics->target(graphics->blur);
            graphics->draw(graphics->color, shader_blur, graphics->ortho, spatial::matrix(), spatial::matrix());
        }

        // Render the visible scene
        graphics->draw(graphics->color, shader_post, graphics->ortho, spatial::matrix(), spatial::matrix());

        // Render anything not subjected to post processing
        graphics->draw(graphics->shadow, graphics->shadow.texture.depth ? assets->get<type::program>("depth_to_color") : shader_basic, graphics->ortho, spatial::matrix(), spatial::matrix().translate(20, graphics->height() - 20 - 256, 0));
        graphics->draw(graphics->depth, graphics->depth.texture.depth ? assets->get<type::program>("depth_to_color") : shader_basic, graphics->ortho, spatial::matrix(), spatial::matrix().translate(40 + 256, graphics->height() - 20 - 256, 0).scale(0.2));
        graphics->draw(graphics->picking, graphics->picking.texture.depth ? assets->get<type::program>("depth_to_color") : shader_basic, graphics->ortho, spatial::matrix(), spatial::matrix().translate(40 + 582, graphics->height() - 20 - 256, 0).scale(0.2));

        //spatial::matrix model = spatial::matrix().translate(pos.eye, pos.center, pos.up);

        /*
        if (object_moving[0]) {
            pos.spin(1.0f);
        }
        if (object_moving[1]) {
            pos.move(0.1f);
        }
        if (object_moving[2]) {
            pos.spin(-1.0f);
        }
        if (object_moving[3]) {
            pos.move(-0.1f);
        }
        if (object_moving[4]) {
            pos.pitch(-1.0f);
        }
        if (object_moving[5]) {
            pos.pitch(1.0f);
        }
        */

        if (camera_moving[0]) {
            camera.surge(1);
        }
        if (camera_moving[1]) {
            camera.surge(-1);
        }
        if (camera_moving[2]) {
            camera.sway(1);
        }
        if (camera_moving[3]) {
            camera.sway(-1);
        }

        /*
        for (auto &projectile : projectiles) {
            spatial::matrix position = spatial::matrix().translate(projectile.eye, projectile.center, projectile.up);
            trail = position.interpolate(spatial::ray(spatial::vector(0.0, 0.0, -0.2), spatial::vector(0.0, 0.0, 0.2)));
            graphics->recompile(trail);

            graphics->draw(trail, shader, perspective, view);

            projectile.move(0.4);

            spatial::matrix model = spatial::matrix().translate(projectile.eye, projectile.center, projectile.up);
            graphics->draw(ray, shader, perspective, view, model);
        }
        */
    }

    float prior_x;
    float prior_y;

    spatial::vector mouse;

    void freelook_start(const platform::input::event& ev) {
        prior_x = ev.point.x;
        prior_y = ev.point.y;
        if (main::global().flag("debug.input")) {
            std::stringstream ss;
            ss << "freelook_start(" << ev.point.x << ", " << ev.point.y << ")";
            main::debug().content.add(ss.str());
        }
    }

    void freelook_move(const platform::input::event& ev) {
        if (main::global().flag("debug.input")) {
            std::stringstream ss;
            ss << "freelook_move(" << ev.identifier << ")(" << ev.point.x << ", " << ev.point.y << ")";
            main::debug().content.add(ss.str());
        }
        camera.pitch(ev.point.y - prior_y);
        camera.spin(prior_x - ev.point.x);
        prior_x = ev.point.x;
        prior_y = ev.point.y;
    }

    void freelook_zoom(const platform::input::event& ev) {
        if (main::global().flag("debug.input")) {
            std::stringstream ss;
            ss << "on_zoom(" << ev.travel << ")(" << ev.point.x << ", " << ev.point.y << ")";
            main::debug().content.add(ss.str());
        }
        camera.surge(ev.travel > 0 ? 1.0 : -1.0);
    }

    void mouse_click(const platform::input::event& ev) {
        if (main::global().flag("debug.input")) {
            std::stringstream ss;
            ss << "mouse_click(" << ev.point.x << ", " << ev.point.y << ")";
            main::debug().content.add(ss.str());
        }

        spatial::position reference;

        auto view = spatial::matrix().lookat(camera.eye, camera.center, camera.up);
        auto& perspective = graphics->perspective;

        spatial::vector position_start = spatial::vector({ ev.point.x, ev.point.y }).unproject(perspective, view, graphics->width(), graphics->height());
        spatial::vector position_end = spatial::vector({ ev.point.x, ev.point.y, 1.0f }).unproject(perspective, view, graphics->width(), graphics->height());

        rays.push_back(spatial::ray(position_start, position_end));

        {
            int w = graphics->width();
            int h = graphics->height();            

            int i = (graphics->width() * (graphics->height() - ev.point.y) * 4) + (ev.point.x * 4);

            int r = graphics->pixels[i];
            int g = graphics->pixels[i+1];
            int b = graphics->pixels[i+2];
            int a = graphics->pixels[i+3];

            std::stringstream ss;
            ss << "mouse_click(" << ev.point.x << ", " << ev.point.y << ") (" << r << "," << g << "," << b << "," << a << ")";
            main::debug().content.add(ss.str());

            /*
            // Store off the data for evaluation
            std::vector<unsigned char> png;
            lodepng::State state;
            unsigned error = lodepng::encode(png, graphics->pixels.data(), graphics->width(), graphics->height(), state);
            if (!error) lodepng::save_file(png, "c:\\temp\\picker.png");
            */
        }

        /*
        
        spatial::vector normalized = { 
            ( 2.0f * ((float)ev.point.x / (float)graphics->width()) ) - 1.0f,  
            1.0f - ( 2.0f * ((float)ev.point.y / (float)graphics->height()) ),
            0.0, 
            1.0 
        };

        // spatial::vector relative = { (float)ev.point.x, (float)ev.point.y, 0.0f };
        // spatial::vector position = relative.project(main::global().perspective, spatial::matrix().lookat(camera.eye, camera.center, camera.up), spatial::matrix());

        spatial::vector position = projection * normalized;

        auto point = camera;

        point.reposition(position);

        //std::stringstream ss;
        //ss << "mouse_click(" << ev.point.x << ", " << ev.point.y << ") -> (" << position.x << "," << position.y << "," << position.z << "," << position.w << ")";
        //main::debug().content.add(ss.str());

        rays.push_back(spatial::ray(point.center, point.eye));

        */

        rays.back().texture.create(1, 1, 255, 255, 255, 255);
        rays.back().xy_projection(0, 0, 1, 1);
        graphics->compile(rays.back());
    }

    void mouse_move(const platform::input::event& ev) {
        if (main::global().flag("debug.input")) {
            std::stringstream ss;
            ss << "mouse_move(" << ev.point.x << ", " << ev.point.y << ")";
            main::debug().content.add(ss.str());
        }
        mouse = ev.point;
        prior_x = ev.point.x;
        prior_y = ev.point.y;
    }

    bool camera_moving[4] = { false, false, false, false };
    bool object_moving[6] = { false, false, false, false, false, false };
    void keyboard_input(const platform::input::event& ev) {
        if (main::global().flag("debug.input")) {
            std::stringstream ss;
            ss << "key(" << ev.identifier << ")";
            main::debug().content.add(ss.str());
        }

        if (ev.gesture == platform::input::DOWN && gui->active() == false) {
            switch (ev.identifier) {
            case(27):
                main::global().transition("game", "title");
                break;
            case(37):
                object_moving[0] = true;
                break;
            case(38):
                object_moving[1] = true;
                break;
            case(39):
                object_moving[2] = true;
                break;
            case(40):
                object_moving[3] = true;
                break;
            case(188):
                object_moving[4] = true;
                break;
            case(190):
                object_moving[5] = true;
                break;
            case(83):
                camera_moving[0] = true;
                break;
            case(87):
                camera_moving[1] = true;
                break;
            case(65):
                camera_moving[2] = true;
                break;
            case(68):
                camera_moving[3] = true;
                break;
            }
        }

        if (ev.gesture == platform::input::UP && gui->active() == false) {
            switch (ev.identifier) {
            case(32):
                //projectiles.push_back(pos);
                //if (projectiles.size() > 10) {
                //    projectiles.pop_front();
                //}
                break;
            }
        }

        if (ev.gesture == platform::input::UP) {
            switch (ev.identifier) {
            case(37):
                object_moving[0] = false;
                break;
            case(38):
                object_moving[1] = false;
                break;
            case(39):
                object_moving[2] = false;
                break;
            case(40):
                object_moving[3] = false;
                break;
            case(188):
                object_moving[4] = false;
                break;
            case(190):
                object_moving[5] = false;
                break;
            case(83):
                camera_moving[0] = false;
                break;
            case(87):
                camera_moving[1] = false;
                break;
            case(65):
                camera_moving[2] = false;
                break;
            case(68):
                camera_moving[3] = false;
                break;
            }
        }
    }

    void gamepad_input(const platform::input::event& ev) {
        if (main::global().flag("debug.input")) {
            std::stringstream ss;
            if (ev.gesture == platform::input::DOWN || ev.gesture == platform::input::HELD) {
                ss << "button_down(" << ev.identifier << ")";
            }
            if (ev.gesture == platform::input::UP) {
                ss << "button_up(" << ev.identifier << ")";
            }
            main::debug().content.add(ss.str());
        }
    }
};

//=====================================================================================================

void prototype::on_startup() {

    graphics->init();
    audio->init();

    /*
    server->handler([](platform::network::client* caller) {
        std::string input(caller->input.begin(), caller->input.end());
        if (main::global().flag("debug.input")) {
            std::stringstream ss;
            ss << "client_message(" << input << ")";
            main::debug().content.add(ss.str());
        }
        audio->start(sound);
    });
    */

    // Hook up the input handlers
    input->handler(platform::input::POINTER, platform::input::DOWN, [](const platform::input::event& ev) { main::global().freelook_start(ev); }, 2);
    input->handler(platform::input::POINTER, platform::input::DRAG, [](const platform::input::event& ev) { main::global().freelook_move(ev); }, 0); 

    input->handler(platform::input::POINTER, platform::input::WHEEL, [](const platform::input::event& ev) { main::global().freelook_zoom(ev); }, 0);
    input->handler(platform::input::POINTER, platform::input::PINCH, [](const platform::input::event& ev) { main::global().freelook_zoom(ev); }, 0);

    input->handler(platform::input::POINTER, platform::input::DOWN, [](const platform::input::event& ev) { main::global().mouse_click(ev); }, 2);
    input->handler(platform::input::POINTER, platform::input::MOVE, [](const platform::input::event& ev) { main::global().mouse_move(ev); }, 0);

    input->handler(platform::input::KEY, platform::input::DOWN, [](const platform::input::event& ev) { main::global().keyboard_input(ev); }, 0);
    input->handler(platform::input::KEY, platform::input::UP, [](const platform::input::event& ev) { main::global().keyboard_input(ev); }, 0);

    input->handler(platform::input::GAMEPAD, platform::input::DOWN, [](const platform::input::event& ev) { main::global().gamepad_input(ev); }, 0);
    input->handler(platform::input::GAMEPAD, platform::input::HELD, [](const platform::input::event& ev) { main::global().gamepad_input(ev); }, 0);
    input->handler(platform::input::GAMEPAD, platform::input::UP, [](const platform::input::event& ev) { main::global().gamepad_input(ev); }, 0);

    main::global().add("debug", new console());
    main::global().add("splash", new splash());
    main::global().add("title", new title());
    main::global().add("game", new game());

    main::global().activate("splash");

    gui->position();

    //server->start();
}

void prototype::on_resize() {
    float scale = main::global().has("blur.scale") ? std::get<double>(main::global().get("blur.scale")) : 0.5;
    graphics->dimensions(width, height, scale);

    int fov = main::global().has("perspective.fov") ? std::get<int>(main::global().get("perspective.fov")) : 90;
    graphics->projection(fov);

    main::global().dimensions(width, height);

    gui->projection = graphics->ortho;

    gui->position();
}

void prototype::on_draw() {
    graphics->clear();

    main::global().run();

    gui->draw();
    graphics->flush();
}

void prototype::on_interval() {
    main::debug().content.add("on_proc");
}
