/*

Book:      	Game and Graphics Programming for iOS and Android with OpenGL(R) ES 2.0
Author:    	Romain Marucchi-Foino
ISBN-10: 	1119975913
ISBN-13: 	978-1119975915
Publisher: 	John Wiley & Sons	

Copyright (C) 2011 Romain Marucchi-Foino

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of
this software. Permission is granted to anyone who either own or purchase a copy of
the book specified above, to use this software for any purpose, including commercial
applications subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that
you wrote the original software. If you use this software in a product, an acknowledgment
in the product would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be misrepresented
as being the original software.

3. This notice may not be removed or altered from any source distribution.

*/

#include "templateApp.h"

#include "chunk.hpp"
/* The main structure of the template. This is a pure C struct, you initialize the structure
   as demonstrated below. Depending on the type of your type of app simply comment / uncomment
   which event callback you want to use. */

TEMPLATEAPP templateApp = {
							/* Will be called once when the program start. */
							templateAppInit,
							
							/* Will be called every frame. This is the best location to plug your drawing. */
							templateAppDraw,
							
							/* This function will be triggered when a new touche is recorded on screen. */
							templateAppToucheBegan,
							
							/* This function will be triggered when an existing touche is moved on screen. */
							templateAppToucheMoved,
							
							/* This function will be triggered when an existing touche is released from the the screen. */
							//templateAppToucheEnded,
							
							/* This function will be called everytime the accelerometer values are refreshed. Please take
							not that the accelerometer can only work on a real device, and not on the simulator. In addition
							you will have to turn ON the accelerometer functionality to be able to use it. This will be
							demonstrated in the book later on. */
							//templateAppAccelerometer // Turned off by default.
						  };


#define VERTEX_SHADER ( char * )"vertex.glsl"

#define FRAGMENT_SHADER ( char * )"fragment.glsl"

#define DEBUG_SHADERS 1

PROGRAM *program = NULL;

MEMORY *m = NULL;

chunk *ch = NULL;

superchunk *schunk = NULL;

vec2 touche = { 0.0f, 0.0f };

vec3 rot_angle = { 45.0f, -45.0f, 0.0f };

void initializeChunk(void)
{
    schunk = new superchunk();
    schunk->random();
//    time_t seed = time(NULL);
//    schunk->noise(seed);
    
//    ch = new chunk();
//
//    ch->random();
}

void templateAppInit( int width, int height )
{
	// Setup the exit callback function.
	atexit( templateAppExit );
	
	// Initialize GLES.
	GFX_start();
	
	// Setup a GLES viewport using the current width and height of the screen.
	glViewport( 0, 0, width, height );
	
	/* Insert your initialization code here */
    GFX_set_matrix_mode( PROJECTION_MATRIX );
	{
		GFX_load_identity();
		
		GFX_set_perspective( 45.0f,
                            ( float )width / ( float )height,
                            0.01f,
                            1000.0f,
                            0.0f );
		
		glDisable( GL_CULL_FACE );
	}
	
	program = PROGRAM_init( ( char * )"default" );
	
	program->vertex_shader = SHADER_init( VERTEX_SHADER, GL_VERTEX_SHADER );
    
	program->fragment_shader = SHADER_init( FRAGMENT_SHADER, GL_FRAGMENT_SHADER );
	
	m = mopen( VERTEX_SHADER, 1 );
	
	if( m ) {
        
		if( !SHADER_compile( program->vertex_shader,
                            ( char * )m->buffer,
                            DEBUG_SHADERS ) ) exit( 1 );
	}
	m = mclose( m );
    
	m = mopen( FRAGMENT_SHADER, 1 );
	
	if( m ) {
        
		if( !SHADER_compile( program->fragment_shader,
                            ( char * )m->buffer,
                            DEBUG_SHADERS ) ) exit( 2 ); 
	}
    
	m = mclose( m );
    
    if( !PROGRAM_link( program, DEBUG_SHADERS ) ) exit( 3 );
    
    
    initializeChunk();
}

float gamma_ = 0.0;

void templateAppDraw( void )
{
    static unsigned int start = get_milli_time(),
    fps = 0;
    if( get_milli_time() - start >= 1000 ) {
        console_print("FPS: %d\n", fps );
        start = get_milli_time();
        fps = 0;
    }
    ++fps;
    
//    printf("templateAppDraw...\n");
    glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
	// Clear the depth, stencil and colorbuffer.
	glClear( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

	/* Insert your drawing code here */
    GFX_set_matrix_mode( MODELVIEW_MATRIX );
	
	GFX_load_identity();
    
	vec3 e = { 0.0f, 0.0f, 1.0f },
    c = { 0.0f,  0.0f, 0.0f },
    u = { 0.0f,  1.0f, 0.0f };
    
	GFX_look_at( &e, &c, &u );
    gamma_ += 0.1;
    
//	static float y = 0.0f;
	
//	y += 0.1f;
//	
	GFX_translate( 0.0f, 0.0, -800.0f + gamma_);
    
    GFX_rotate( rot_angle.x, 1.0f, 0.0f, 0.0f );
	GFX_rotate( rot_angle.y, 0.0f, 1.0f, 0.0f );
//
//	GFX_rotate( y * 50.0f,
//               1.0f,
//               1.0f,
//               1.0f );
    
	if( program->pid ) {
        
//		char attribute, uniform;
//		
//		glUseProgram( program->pid );
//        
//		uniform = PROGRAM_get_uniform_location( program,
//                                               ( char * )"mvp" );
//		
//		glUniformMatrix4fv( uniform,
//                           1 /* How many 4x4 matrix */,
//                           GL_FALSE /* Transpose the matrix? */,
//                           ( float * )GFX_get_modelview_projection_matrix() );
//        
//		attribute = PROGRAM_get_vertex_attrib_location( program,
//                                                       ( char * )"coord" );
//        
//		glEnableVertexAttribArray( attribute );
//        ch->render(attribute);
        
        
        
        
        
		glUseProgram( program->pid );
        schunk->render(program);

        
        
        
        
        
        
//        glVertexAttribPointer(attribute_coord, 4, GL_BYTE, GL_FALSE, 0, 0);
//        
//        glDrawArrays(GL_TRIANGLES, 0, elements);
        
        
//
//		glVertexAttribPointer( attribute, 3, GL_FLOAT, GL_FALSE, 0, POSITION );
//		
//		attribute = PROGRAM_get_vertex_attrib_location( program,
//                                                       ( char * )"COLOR" );
//        
//		glEnableVertexAttribArray( attribute );
//		
//		glVertexAttribPointer( attribute, 4, GL_FLOAT, GL_FALSE, 0, COLOR );
//        
//		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	}
}


void templateAppToucheBegan( float x, float y, unsigned int tap_count ) {
    
	touche.x = x;
	touche.y = y;
}


void templateAppToucheMoved( float x, float y, unsigned int tap_count ) {
    
	rot_angle.y += -( touche.x - x );
	rot_angle.x += -( touche.y - y );
    
	touche.x = x;
	touche.y = y;
}



void templateAppToucheEnded( float x, float y, unsigned int tap_count )
{
	/* Insert code to execute when a touche is removed from the screen. */
}


void templateAppAccelerometer( float x, float y, float z )
{
	/* Insert code to execute with the accelerometer values ( when available on the system ). */
}


void templateAppExit( void )
{
	/* Code to run when the application exit, perfect location to free everything. */
    if( program && program->vertex_shader )
		program->vertex_shader = SHADER_free( program->vertex_shader );
    
	if( program && program->fragment_shader )
		program->fragment_shader = SHADER_free( program->fragment_shader );
    
	if( program )
		program = PROGRAM_free( program );
    
    if (schunk) {
        delete schunk;
    }
}





