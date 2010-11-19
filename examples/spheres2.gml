% spheres2.gml
%
% A pair of spheres over a white plane
%

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
} plane /p

% { /v /u /face blue 1.0 0.8 1.0 } plane /p

				% a sphere
{ /v /u /face			  % bind arguments
  0.8 0.2 v point		  % surface color
  1.0 0.8 3.0
} sphere /s

				% a matte white plane
%{ /v /u /face
%  1.0 1.0 1.0 point
%  1.0 0.0 1.0
%} plane /p

				% scene consisting of two spheres
s -1.2 0.0 4.0 translate 	  % sphere at (-1, 0, 3)
1.0 1.5 1.0 scale
s  1.2 1.0 4.0 translate	  % sphere at (1, 1, 3)
p 0.0 -1.0 0.0 translate
union union /scene		  % compose

				% directional light
%1.0 -1.0 0.0 point		  % direction
%0.5  0.5 0.5 point light /l	  % directional light

%-10 10 0 point
%1.0 1.0 1.0 point pointlight /l

-5 5 0 point		% pos
1.2 1.0 4.0 point	% at
1.0 1.0 1.0 point	% color
10.0			% cutoff (degrees)
2.0			% exp
spotlight /l

%
% Debug hook to determine which pixels to trace.
%
%{ /y /x 
%  140 y eqi
%} /trace-pel-p

%{ /y /x true } /trace-pel-p

				% render
0.2 0.2 0.2 point		  % ambient light
[ l ]				  % lights
scene				  % scene to render
5				  % tracing depth
90.0				  % field of view
640 480 % image wid and height
"spheres2.ppm"			  % output file
render

