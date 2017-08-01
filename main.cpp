#include <SFML/Graphics.hpp>
#include<SFML/Audio.hpp>
#include<iostream>
#include<math.h>
#include <time.h>
#include <list>
#include <sstream>
using namespace sf;

const int W = 800;
const int H = 800;

float DEGTORAD = 0.017453f;

class Animation
{
public:
	float Frame, speed;
	Sprite sprite;

    std::vector<IntRect> frames;

	Animation(){}

    Animation (Texture &t, int x, int y, int w, int h, int count, float Speed)
	{
	    Frame = 0;
        speed = Speed;

		for (int i=0;i<count;i++)
         frames.push_back( IntRect(x+i*w, y, w, h)  );

		sprite.setTexture(t);
		sprite.setOrigin(w/2,h/2);
        sprite.setTextureRect(frames[0]);
	}


	void update()
	{
    	Frame += speed;
		int n = frames.size();
		if (Frame >= n) Frame -= n;
		if (n>0) sprite.setTextureRect( frames[int(Frame)] );
	}

	bool isEnd()
	{
	  return Frame+speed>=frames.size();
	}

};


class Entity
{
public:
float x,y,dx,dy,R,angle;
int  life;  //life is boolean meaning that is only one time life:: can make it a integer say 5 lives
std::string name;
Animation anim;

Entity()
{
  life=1;
}

void settings(Animation &a,int X,int Y,float Angle=0,int radius=1)
{
  anim = a;
  x=X; y=Y;
  angle = Angle;
  R = radius;
}

virtual void update(){};

void draw(RenderWindow &app)
{
  anim.sprite.setPosition(x,y);
  anim.sprite.setRotation(angle+90);
  app.draw(anim.sprite);

  CircleShape circle(R);
  circle.setFillColor(Color(255,0,0,170));
  circle.setPosition(x,y);
  circle.setOrigin(R,R);
  app.draw(circle);
}

virtual ~Entity(){};
};


class asteroid: public Entity
{
public:
  asteroid()
  {
    dx=rand()%5+1;
    dy=rand()%5+1
    ;
    name="asteroid";
  }

void  update()
  {
   x+=dx;
   y+=dy;

   if (x>W) x=0;  if (x<0) x=W;
   if (y>H) y=0;  if (y<0) y=H;
  }

};

Clock clk;
class oneup: public Entity // PASTE IT BOSSSSSSSS
{
public:
    oneup()
    {
        name="oneup";
    }
};

class bullet: public Entity  //bullet object
{
public:
  bullet()
  {
    name="bullet";
  }

void  update()
  {
   dx=cos(angle*DEGTORAD)*6;//speed
   dy=sin(angle*DEGTORAD)*6;//speed
  // angle+=rand()%6-3;
   x+=dx;
   y+=dy;

   if (x>W || x<0 || y>H || y<0) life=0;
  }

};

class boss:public Entity
{
public:
    boss()
    {
        name="boss";
    }
};

class player: public Entity
{
public:
   bool thrust;

   player()
   {
     name="player";
     life=5;
   }

   void update()
   {
     if (thrust)
      { dx+=cos(angle*DEGTORAD)*0.2;
        dy+=sin(angle*DEGTORAD)*0.2;
       }
     else
      { dx*=0.5;
        dy*=0.5;//originally 0.99
      }

    int maxSpeed=8;
    float speed = sqrt(dx*dx+dy*dy);
    if (speed>maxSpeed)
     { dx *= maxSpeed/speed;
       dy *= maxSpeed/speed;
     }

    x+=dx;
    y+=dy;

    if (x>W) x=0; if (x<0) x=W;
    if (y>H) y=0; if (y<0) y=H;
   }

};


bool isCollide(Entity *a,Entity *b)
{
  return (b->x - a->x)*(b->x - a->x)+
         (b->y - a->y)*(b->y - a->y)<
         (a->R + b->R)*(a->R + b->R);
}


int main()
{

    srand(time(0));
    //making starting window first//
//    RenderWindow app2(VideoMode(W,H),"Opening");
//    app2.setFramerateLimit(10);
//   while(app2.isOpen())
//   {
//       Event event;
//
//    while(app2.pollEvent(event))
//    {
//       if (event.type == Event::Closed)
//                app2.close();
//
//      if (event.type == Event::KeyPressed && event.key.code == Keyboard::S)
//
//        {
//            app2.close();
//            break;
//
//        }
//    }
//   }

     //Starting window
    RenderWindow app(VideoMode(W, H,64), "Asteroids!");
    //app.setVisible(false);
    //app.close();
    app.setFramerateLimit(60);

    //different background images
    Texture t1,t2,t3,t4,t5,t6,t7,heart,frSc,oup,lv3,bosstxt;
    bosstxt.loadFromFile("images/Boss.png");
    lv3.loadFromFile("images/Level3.jpg");
    frSc.loadFromFile("images/a.jpg");
    t1.loadFromFile("images/spaceship.png");
    t2.loadFromFile("images/background.jpg");
    t3.loadFromFile("images/explosions/type_C.png");
    t4.loadFromFile("images/rock.png");
    t5.loadFromFile("images/fire_blue.png");
    t6.loadFromFile("images/rock_small.png");
    t7.loadFromFile("images/explosions/type_B.png");
    heart.loadFromFile("images/heart.png");
    oup.loadFromFile("images/oneup.png");

    t1.setSmooth(true);
    t2.setSmooth(true);

    Sprite background(t2);
    Sprite frontScreen(frSc);
    Sprite lv3bg(lv3);
    //stateVariables
    int isPlaying=0;


    //Animation module

   //Animation (Texture &t, int x, int y, int w, int h, int count, float Speed)
    Animation sExplosion(t3, 0,0,256,256, 48, 0.5);
    Animation sRock(t4, 0,0,64,64, 16, 0.2);
    Animation sRock_small(t6, 0,0,64,64, 16, 0.2);
    Animation sBullet(t5, 0,0,32,64, 16, 0.8);
    Animation sPlayer(t1, 40,0,40,40, 1, 0);
    Animation sPlayer_go(t1, 40,40,40,40, 1, 0);
    Animation sExplosion_ship(t7, 0,0,192,192, 64, 0.5);
    Animation Life_heart(heart,0,0,80,80,1,0);
    Animation anione(oup,0,0,66,66,1,0);
    Animation aniboss(bosstxt,0,0,180,180,1,0);

    std::list<Entity*> entities;
    std::list<Entity*> lv3ent;

     //add astroids
    //void settings(Animation &a,int X,int Y,float Angle=0,int radius=1)
    for(int i=0;i<5;i++)
    {
      asteroid *a = new asteroid();
      a->settings(sRock, rand()%W/2, rand()%H/2, rand()%360, 25);
      entities.push_back(a);
    }
    //add player

    player *p = new player();
    p->settings(sPlayer,200,350,0,20);
    entities.push_back(p);
    //p->settings(sPlayer,0,350,0,20);
    player *p2=new player();
    p2->settings(sPlayer,50,350,0,20);
    lv3ent.push_back(p2);

    boss *bs=new boss();
    bs->settings(aniboss,600,350,-90,100);
    lv3ent.push_back(bs);


    /////Sound Buffer for level 1 sound using 09.ogg
    sf::SoundBuffer ballSoundBuffer;
    ballSoundBuffer.loadFromFile("resources/09.ogg");
    sf::Sound ballSound(ballSoundBuffer);

    /////Sound Buffer for start screen sound using 03.ogg
    sf::SoundBuffer startsoundBuffer;
    startsoundBuffer.loadFromFile("resources/03.ogg");
    sf::Sound startsound(startsoundBuffer);

    /////Sound Buffer for level3 sound using BossBattle.ogg
    sf::SoundBuffer BossBattleBuffer;
    BossBattleBuffer.loadFromFile("resources/BossBattle.ogg");
    sf::Sound BossBattlesound(BossBattleBuffer);

     //sound for bullets firing//
    sf:: SoundBuffer Collision1Buffer;
    if(!Collision1Buffer.loadFromFile("resources/Laser_Shoot2.wav"))
        return EXIT_FAILURE;
    sf:: Sound collision1(Collision1Buffer);

    //sound when powerup//
    sf:: SoundBuffer Collision2Buffer;
   if(!Collision2Buffer.loadFromFile("resources/PowerUp7.wav"))
      return EXIT_FAILURE;
    sf:: Sound collision2(Collision2Buffer);

    //sound for explosion between spaceship and asteroid
    sf:: SoundBuffer Collision3Buffer;
    if(!Collision3Buffer.loadFromFile("resources/Explosion9.wav"))
        return EXIT_FAILURE;
    sf:: Sound collision3(Collision3Buffer);

    //sound for collion between bullet and asteroids
    sf:: SoundBuffer Collision4Buffer;
    if(!Collision4Buffer.loadFromFile("resources/Randomize4.wav"))
        return EXIT_FAILURE;
    sf:: Sound collision4(Collision4Buffer);


    //fonts
     sf::Font f;
     if(f.loadFromFile("data/jokerman.ttf")==0)
     {
         return 0;
     }
     ///////TEXT////////
     //frontmsg
     //startmsg
     //changed incorporated//
      sf::Text start2;
     start2.setString("Welcome Gamers");
     start2.setCharacterSize(50);
     start2.setPosition(170,10);
     start2.setStyle(sf::Text::Bold);
     start2.setFont(f);
     start2.setFillColor(sf::Color::Red);

     sf::Text start;
     start.setString("START GAME(S)");
     start.setCharacterSize(50);
     start.setPosition(250,250);
     start.setStyle(sf::Text::Bold);
     start.setFont(f);
     start.setFillColor(sf::Color::Yellow);
     //exitmsg
     sf::Text exit;
     exit.setString("EXIT GAME(X)");
     exit.setCharacterSize(50);
     exit.setPosition(250,320);
     exit.setStyle(sf::Text::Bold);
     exit.setFont(f);
     exit.setColor(sf::Color::Yellow);
     //score
     int score=0;

     std::ostringstream ssScr;
     ssScr<<"SCORE : "<<score;

     sf::Text scr;

     scr.setString("SCORE : 0");
     scr.setFont(f);
     scr.setCharacterSize(15);
     scr.setPosition(W-150,10);

     //life
     sf::Text life;
     ssScr.str("");
     Entity *lif=new Entity();
     lif->settings(Life_heart,30,30,-90);

     life = scr;
     ssScr<<"LIFE : "<<p->life;
     life.setPosition(10,10);
     life.setString(ssScr.str());
     entities.push_back(lif);
     //scr.setColor(sf::Color::White);

     ///////start/////////////
//        RenderWindow app2(VideoMode(W, H), "opening!");
//   app2.setFramerateLimit(10);
//   while(app2.isOpen())
//   {
//       Event event;
//
//    while(app2.pollEvent(event))
//    {
//       if (event.type == Event::Closed)
//                app2.close();
//
//      if (event.type == Event::KeyPressed && event.key.code == Keyboard::S)
//
//        {
//            app2.close();
//
//        }
//    }
//   }

    /////main loop/////
    app.setActive(true);
    app.setVisible(true);

        int counter=0;
        int av=0; // to denote whether one up is present on the screen or not
        int soundon=0; // to denote whether level 1 sound is playing or not

        //Start sound play starts
        startsound.play();
        startsound.setLoop(1);


int l3s=0;
    while (app.isOpen())
    {
        if(!av)
        counter++;
        if(counter>500 && !av)
        {
            av=1;
            counter=0;
            int rx=rand()%400;
        int ry=rand()%400;
        oneup *o=new oneup();
        o->settings(anione,rx,ry,-90,20);
        entities.push_back(o);
        }
        Event event;
        while (app.pollEvent(event))
        {
            if (event.type == Event::Closed)
                app.close();
            if(score>=1000 && !l3s)
                {
                    l3s=1;
                    isPlaying=2;
                    ballSound.stop();
                    soundon=0;
                    //entities.clear();
                }
            if(!isPlaying)
            {
                if (event.type == Event::KeyPressed)
                if (event.key.code == Keyboard::S)
                {
                    isPlaying=1;
                    startsound.stop();
                }
                if (event.type == Event::KeyPressed)
                if (event.key.code == Keyboard::X)
                {
                    app.close();
                }
            }
            if(isPlaying)
            {

                if (event.type == Event::KeyPressed)
                if (event.key.code == Keyboard::Space)
              {
                bullet *b = new bullet();
                collision1.play();
                if(isPlaying==1)
                {
                    b->settings(sBullet,p->x,p->y,p->angle,10);
                    entities.push_back(b);
                }
                else if(isPlaying==2)
                {
                    b->settings(sBullet,p2->x,p2->y,p2->angle,10);
                    lv3ent.push_back(b);
                }

              }
            }

        }

        if(isPlaying==1)
        {
               if (Keyboard::isKeyPressed(Keyboard::Right)) p->angle+=5;
               if (Keyboard::isKeyPressed(Keyboard::Left))  p->angle-=5;
               if (Keyboard::isKeyPressed(Keyboard::Up)) p->thrust=true;
               else p->thrust=false;

               for(auto a:entities)
                for(auto b:entities)
            {
            if (a->name=="asteroid" && b->name=="bullet")
            if ( isCollide(a,b) )
           {
            a->life=false;
            b->life=false;
             collision4.play();
            score+=100;

            ssScr.str("");
            std::cout<<"SCORE:"<<score<<" %%";

            ssScr<<"SCORE : "<<score;

            scr.setString(ssScr.str());
            Entity *e = new Entity();
            e->settings(sExplosion,a->x,a->y);
            e->name="explosion";
            entities.push_back(e);


            for(int i=0;i<2;i++)
            {
             if (a->R==15) continue;
             Entity *e = new asteroid();
             e->settings(sRock_small,a->x,a->y,rand()%360,15);
             entities.push_back(e);
            }

           }

        if (a->name=="player" && b->name=="asteroid")
        if ( isCollide(a,b) )
           {
            b->life=false;
               collision3.play();

            Entity *e = new Entity();
            e->settings(sExplosion_ship,a->x,a->y);
            e->name="explosion";

            entities.push_back(e);
            p->life--;
            ssScr.str("");
            ssScr<<"LIFE : "<<p->life;
            life.setString(ssScr.str());
            std::cout<< p->life <<" ";
            if(p->life <= 0)
            {
                app.close();
                p->settings(sPlayer,W/2,H/2,0,20);
                p->dx=0; p->dy=0;
            }

            }
            if (a->name=="player" && b->name=="oneup")  // PASTE IT BOSSSSS 373-383
            if ( isCollide(a,b) )
           {
               av=0;
            b->life=false;
            p->life++;
              collision2.play();
            ssScr.str("");
            ssScr<<"LIFE : "<<p->life;
            life.setString(ssScr.str());
            std::cout<< p->life <<" ";
           }
        }


        if (p->thrust)  p->anim = sPlayer_go;
        else   p->anim = sPlayer;




        for(auto e:entities)
        if (e->name=="explosion")
        if (e->anim.isEnd()) e->life=0;

        if (rand()%150==0)
        {
            asteroid *a = new asteroid();
            a->settings(sRock, 0,rand()%H, rand()%360, 25);
            entities.push_back(a);
        }

        for(auto i=entities.begin();i!=entities.end();)
        {
            Entity *e = *i;

            e->update();
            e->anim.update();

            if (e->life==false) {i=entities.erase(i); delete e;}
            else i++;
        }

    }
    if(isPlaying==2)
    {
         p2->life=p->life;

         if (Keyboard::isKeyPressed(Keyboard::Right)) p2->angle+=5;
               if (Keyboard::isKeyPressed(Keyboard::Left))  p2->angle-=5;
               if (Keyboard::isKeyPressed(Keyboard::Up)) p2->thrust=true;
               else p2->thrust=false;

                if (p2->thrust)  p2->anim = sPlayer_go;
        else   p2->anim = sPlayer;

        for(auto i=lv3ent.begin();i!=lv3ent.end();)
        {
            Entity *e = *i;

            e->update();
            e->anim.update();

            if (e->life==false) {i=entities.erase(i); delete e;}
            else i++;
        }

    }

//////sound/////
if(!soundon && isPlaying)
     {
         soundon=1;
         if(isPlaying==1)
         {
             ballSound.play();
            ballSound.setLoop(1);
         }
         else if(isPlaying==2)
         {
             soundon=1;
             BossBattlesound.play();
             BossBattlesound.setLoop(1);
         }
     }

   //////draw//////
   app.clear();
   if(isPlaying==1)
   {
       //////Level 1 Music 09/////


        app.draw(background);
   app.draw(scr);
   app.draw(life);
   for(auto i:entities)
     i->draw(app);
   app.draw(life);
   }
   else if(!isPlaying)
   {
       app.draw(frontScreen);
       app.draw(start);
       app.draw(exit);
       app.draw(start2);
   }
   else if(isPlaying==2)
   {
       app.draw(lv3bg);
       app.draw(scr);
   app.draw(life);
   for(auto i:lv3ent)
     i->draw(app);
   app.draw(life);
   }
   app.display();

    }

   RenderWindow app1(VideoMode(W, H), "CLOSING!");
   app1.setFramerateLimit(10);
   while(app1.isOpen())
   {
       Event event;

    while(app1.pollEvent(event))
    {
       if (event.type == Event::Closed)
                app1.close();

          if (event.type == Event::KeyPressed)
        if (event.key.code == Keyboard::R)
        {
            app1.close();
            main();
        }

    }
   }


    return 0;
}
