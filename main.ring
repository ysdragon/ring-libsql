# The Main File
load "package.ring"
load "lib.ring"
load "src/utils/color.ring"

# Constants
BOX_WIDTH = 47
BORDER_COLOR = :BRIGHT_BLUE
BORDER_STYLE = :BOLD

func main
	displayBanner()

func displayBanner
	? colorBox("╭" + copy("─", BOX_WIDTH) + "╮", BORDER_COLOR, BORDER_STYLE)
	? colorBox("│" + copy(" ", BOX_WIDTH) + "│", BORDER_COLOR, NULL)
	? centerInBox(aPackageInfo[:name], :CYAN, :BOLD, BOX_WIDTH, BORDER_COLOR)
	? colorBox("│" + copy(" ", BOX_WIDTH) + "│", BORDER_COLOR, NULL)
	? centerInBox("Version " + aPackageInfo[:version], :YELLOW, :BOLD, BOX_WIDTH, BORDER_COLOR)
	? centerInBox("Author: Youssef Saeed (ysdragon)", :MAGENTA, NULL, BOX_WIDTH, BORDER_COLOR)
	? centerInBox("https://github.com/ysdragon", :GREEN, :UNDERLINE, BOX_WIDTH, BORDER_COLOR)
	? colorBox("│" + copy(" ", BOX_WIDTH) + "│", BORDER_COLOR, NULL)
	? colorBox("╰" + copy("─", BOX_WIDTH) + "╯", BORDER_COLOR, BORDER_STYLE)
