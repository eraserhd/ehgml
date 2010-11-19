% Test of the cube model.
% The plane makes it easier to identify problems.

{ /v /u /face
  1.0 0.5 0.5 point
  0.3 0.85 1.0
} cylinder
90.0 rotatez
0.0 -0.5 4.0 translate
2.0 uscale
45.0 rotatex
135.0 rotatey /c

1.0  1.0  1.0  point /white
0.0  0.0  1.0  point /blue

[                                 % 3x3 pattern
	[ blue  white ]
    	[ white blue ]
] /texture
{ /i 
  i 0.0 lessf { i negf 0.5 addf } { i } if 
} /fabs

{ fabs apply /v 
  fabs apply /u 
  /face                     % bind parameters
  {                               % toIntCoord : float -> int
    frac 0.5 addf floor /i  % i = floor(3.0*i)
    % i 3 eqi { 2 } { i } if        % return max(2, i)
    i
  } /toIntCoord
  texture u toIntCoord apply get  % color = texture[u][v]
    v toIntCoord apply get
  0.3                             % kd = 1.0
  0.9                            % ks = 0.0
  1.0                             % n = 1.0
} plane
0.0 -3.0 0.0 translate
/p

{ /v /u /face
  0.5 0.5 0.5 point
  0.3 0.85 1.0
} plane 
0.0 0.0 8.0 translate
270.0 rotatex
45.0 rotatez
/p2

c p union p2 union /scene

-10 10 0 point
1.0 1.0 1.0 point pointlight /l

%{ /y /x
%  x 159 eqi {
%    y 93 eqi {
%      true
%    } {
%      y 94 eqi
%    } if
%  } {
%    false
%  } if
%} /trace-pel-p

0.2 0.2 0.2 point		  % ambient light
[ l ]				  % lights
scene				  % scene to render
7				  % tracing depth
90.0				  % field of view
800 600 			  % image wid and height
"cylinder.ppm"			  % output file
render

