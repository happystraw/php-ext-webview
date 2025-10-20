#!/bin/bash

set -euo pipefail

# Default configuration
DEFAULT_EXPORT_DIR="buildroot/lib/pkgconfig"
DEFAULT_PACKAGES="gtk4 webkitgtk-6.0"
DEFAULT_PKG_CONFIG_PATHS=$(pkg-config --variable=pc_path pkg-config)

# Show help information
show_help() {
    cat << EOF
Usage: $0 [options]

Copy pkg-config files and their dependencies to export directory

Options:
    -d, --dir DIR           Specify export directory (default: $DEFAULT_EXPORT_DIR)
    -p, --packages LIST     Specify package list to copy, space separated (default: $DEFAULT_PACKAGES)
    -s, --search-paths PATH Specify pkg-config search paths, colon separated (default: $DEFAULT_PKG_CONFIG_PATHS)
    -v, --verbose           Show verbose output
    -h, --help              Show this help information

Examples:
    $0
    $0 --dir /path/to/export --packages "gtk4 webkitgtk-6.0"
    $0 -v --search-paths "/usr/lib/pkgconfig:/usr/share/pkgconfig:/usr/lib/pkgconfig"

EOF
}

# Parse command line arguments
EXPORT_DIR="$DEFAULT_EXPORT_DIR"
PACKAGES="$DEFAULT_PACKAGES"
PKG_CONFIG_PATHS="$DEFAULT_PKG_CONFIG_PATHS"
VERBOSE=false

while [[ $# -gt 0 ]]; do
    case $1 in
        -d|--dir)
            EXPORT_DIR="$2"
            shift 2
            ;;
        -p|--packages)
            PACKAGES="$2"
            shift 2
            ;;
        -s|--search-paths)
            PKG_CONFIG_PATHS="$2"
            shift 2
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -h|--help)
            show_help
            exit 0
            ;;
        *)
            echo "Error: Unknown option $1" >&2
            show_help >&2
            exit 1
            ;;
    esac
done

# Logging functions
log() {
    if [[ "$VERBOSE" == true ]]; then
        echo "[INFO] $*" >&2
    fi
}

error() {
    echo "[ERROR] $*" >&2
}

warn() {
    echo "[WARN] $*" >&2
}


# Get the full path of .pc file
find_pc_file() {
    local package="$1"
    local search_paths="$2"

    # Convert search paths to array
    IFS=':' read -ra paths <<< "$search_paths"

    for path in "${paths[@]}"; do
        local pc_file="$path/$package.pc"
        if [[ -f "$pc_file" ]]; then
            echo "$pc_file"
            return 0
        fi
    done

    return 1
}

# Parse dependencies from .pc file
parse_pc_dependencies() {
    local pc_file="$1"
    local deps=()

    if [[ ! -f "$pc_file" ]]; then
        return 1
    fi

    # Parse Requires and Requires.private fields
    while IFS= read -r line; do
        if [[ "$line" =~ ^Requires(\.private)?:[[:space:]]*(.*) ]]; then
            local requires="${BASH_REMATCH[2]}"

            # Use more precise method to parse package names and version constraints
            # Handle both comma-separated and space-separated dependencies
            local pkg_specs=()

            # First split by commas, then by spaces for each comma-separated group
            IFS=',' read -ra comma_groups <<< "$requires"
            for group in "${comma_groups[@]}"; do
                # Remove leading and trailing whitespace from the group
                group=$(echo "$group" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')

                # Split the group by spaces
                read -ra space_parts <<< "$group"
                for part in "${space_parts[@]}"; do
                    # Skip empty entries
                    if [[ -z "$part" ]]; then
                        continue
                    fi

                    # Extract package name (first word before any version constraint symbols)
                    local pkg_name
                    pkg_name=$(echo "$part" | sed -E 's/[[:space:]]*(>=|<=|>|<|=).*//' | awk '{print $1}')

                    # Only add non-empty package names without version numbers
                    if [[ -n "$pkg_name" && "$pkg_name" != *"="* && "$pkg_name" != *">"* && "$pkg_name" != *"<"* && ! "$pkg_name" =~ ^[0-9]+\.[0-9]+ ]]; then
                        pkg_specs+=("$pkg_name")
                    fi
                done
            done

            # Add all parsed package names to deps array
            for pkg_name in "${pkg_specs[@]}"; do
                deps+=("$pkg_name")
            done
        fi
    done < "$pc_file"

    printf '%s\n' "${deps[@]}" | sort -u
}

# Recursively collect all dependent .pc files
collect_all_dependencies() {
    local packages=("$@")
    local collected=()
    local to_process=("${packages[@]}")
    local processed=()
    declare -A package_level  # Track the level of each package

    echo "Starting scan for root packages and dependencies..." >&2
    echo "" >&2

    while [[ ${#to_process[@]} -gt 0 ]]; do
        local current="${to_process[0]}"
        to_process=("${to_process[@]:1}")  # Remove first element

        # Check if already processed
        if [[ " ${processed[*]} " =~ " ${current} " ]]; then
            continue
        fi

        processed+=("$current")

        # Determine current package level
        local level=${package_level[$current]:-0}
        local prefix=""
        for ((i=0; i<level; i++)); do
            prefix="   $prefix"
        done

        # Find .pc file
        local pc_file
        if pc_file=$(find_pc_file "$current" "$PKG_CONFIG_PATHS"); then
            collected+=("$pc_file")

            # Print package information (output to stderr)
            if [[ $level -eq 0 ]]; then
                echo "📦 Root package: $current" >&2
            else
                echo "${prefix}└─ $current" >&2
            fi
            echo "${prefix}   File: $pc_file" >&2

            # Parse dependencies
            local deps
            if deps=$(parse_pc_dependencies "$pc_file"); then
                local dep_count=0
                local dep_list=()

                while IFS= read -r dep; do
                    if [[ -n "$dep" ]]; then
                        dep_list+=("$dep")
                        ((dep_count++))
                    fi
                done <<< "$deps"

                if [[ $dep_count -gt 0 ]]; then
                    echo "${prefix}   Requires: ${dep_list[*]}" >&2

                    # Add unprocessed dependencies to queue
                    for dep in "${dep_list[@]}"; do
                        if [[ ! " ${processed[*]} " =~ " ${dep} " ]]; then
                            to_process+=("$dep")
                            package_level[$dep]=$((level + 1))
                        fi
                    done
                else
                    echo "${prefix}   Requires: (no dependencies)" >&2
                fi
            else
                echo "${prefix}   Requires: (parse failed)" >&2
            fi
            echo "" >&2
        else
            echo "${prefix}⚠️  Package not found: $current" >&2
            echo "" >&2
        fi
    done

    printf '%s\n' "${collected[@]}" | sort -u
}

# Copy .pc files to target directory
copy_pc_files() {
    local pc_files=("$@")
    local target_dir="$EXPORT_DIR"

    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo ""
    echo "📁 Copying files to target directory..."
    echo "   Target: $target_dir"
    echo ""

    # Create target directory
    if ! mkdir -p "$target_dir"; then
        error "Failed to create directory: $target_dir"
        exit 1
    fi

    # Copy files
    local copied_count=0
    local failed_count=0

    for pc_file in "${pc_files[@]}"; do
        if [[ -f "$pc_file" ]]; then
            local filename
            filename=$(basename "$pc_file")
            local target_file="$target_dir/$filename"

            if cp "$pc_file" "$target_file"; then
                if [[ "$VERBOSE" == true ]]; then
                    echo "  ✓ $filename"
                fi
                copied_count=$((copied_count + 1))
            else
                echo "  ✗ $filename (copy failed)"
                failed_count=$((failed_count + 1))
            fi
        fi
    done

    echo ""
    if [[ $failed_count -eq 0 ]]; then
        echo "✅ Successfully copied $copied_count .pc files to $target_dir"
    else
        echo "⚠️  Copy completed: $copied_count succeeded, $failed_count failed"
    fi
}

# Verify copied files
verify_copied_files() {
    local target_dir="$EXPORT_DIR"
    # Convert space-separated package list string to array
    local packages
    read -ra packages <<< "$PACKAGES"

    echo ""
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo ""
    echo "🔍 Verifying root package availability..."
    echo ""

    # Set temporary PKG_CONFIG_PATH for testing
    export PKG_CONFIG_PATH="$target_dir"

    local all_ok=true
    for package in "${packages[@]}"; do
        if pkg-config --exists "$package" 2>/dev/null; then
            echo "  ✅ $package: available"
            if [[ "$VERBOSE" == true ]]; then
                echo "     Version: $(pkg-config --modversion "$package")"
                echo "     CFLAGS: $(pkg-config --cflags "$package" 2>/dev/null | head -c 60)..."
                echo "     LIBS: $(pkg-config --libs "$package" 2>/dev/null | head -c 60)..."
                echo ""
            fi
        else
            echo "  ❌ $package: not available"
            all_ok=false
        fi
    done

    echo ""
    if [[ "$all_ok" == true ]]; then
        echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
        echo ""
        echo "🎉 All root packages verified successfully!"
        echo ""
    else
        echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
        echo ""
        warn "⚠️  Some root packages failed verification, please check dependencies"
        echo ""
    fi
}

main() {
    echo "pkg-config File Copy Tool"
    echo "========================="
    echo ""

    # Display configuration
    echo "Configuration:"
    echo "  Export directory: $EXPORT_DIR"
    echo "  Root packages: $PACKAGES"
    echo "  PKG_CONFIG search paths: $PKG_CONFIG_PATHS"
    echo ""
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo ""

    # Collect all dependencies
    local packages
    read -ra packages <<< "$PACKAGES"
    local all_pc_files
    if ! all_pc_files=$(collect_all_dependencies "${packages[@]}"); then
        error "Failed to collect dependencies"
        exit 1
    fi

    if [[ -z "$all_pc_files" ]]; then
        error "No .pc files found"
        exit 1
    fi

    # Show summary of found files
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo ""
    echo "📋 Summary of found .pc files (total: $(echo "$all_pc_files" | wc -l)):"
    echo ""
    while IFS= read -r pc_file; do
        echo "  • $(basename "$pc_file")"
    done <<< "$all_pc_files"

    # Copy files
    echo ""
    local pc_files_array
    readarray -t pc_files_array <<< "$all_pc_files"
    copy_pc_files "${pc_files_array[@]}"

    # Verify
    echo ""
    verify_copied_files
}

main "$@"
