cmake_minimum_required (VERSION 2.6) 
project (DGX-Autopilot)
include(ExternalProject)

ExternalProject_Add(DGX-Autopilot SOURCE_DIR projs
						BINARY_DIR projs
						INSTALL_COMMAND "")


ExternalProject_Add(DGX-Autopilot-demo-app  SOURCE_DIR apps/demo_app
											BINARY_DIR apps/demo_app
										    DEPENDS DGX-Autopilot
											CMAKE_ARGS -Ddgx_DIR=../../projs
											INSTALL_COMMAND "")

ExternalProject_Add(DGX-Autopilot-cockpit-player SOURCE_DIR apps/cockpit_player
												 BINARY_DIR apps/cockpit_player
												 DEPENDS DGX-Autopilot
												 CMAKE_ARGS -Ddgx_DIR=../../projs
												 INSTALL_COMMAND "")

